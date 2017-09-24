/* Copyright (c) 2017 Unpause, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3.0 of the License, or (at your option) any later version.
 *  See the file LICENSE included with this distribution for more
 *  information.
 */

#include <skycoin/net/tcp/connection.hh>
#include <skycoin/log.hh>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/eventfd.h>
#include <sys/prctl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

namespace skycoin { namespace tcp {
    connection::connection(int fd, std::string out_addr, int port)
    : addr_out_(out_addr)
    , pipe_max_size_(4096)
    , port_out_(port)
    , fd_in_(fd)
    , fd_out_(-1)
    {
        pipe_size_[0] = 0;
        pipe_size_[1] = 0;
        memset(fd_pipe_, 0, sizeof(fd_pipe_));

    }
    connection::~connection() {
        if(unregister_handler_) {
            unregister_handler_(fd_in_);
            unregister_handler_(fd_out_);
        }
        for(int i = 0 ; i < 4; i++) {
            if(fd_pipe_[i] > 0) {
                close(fd_pipe_[i]);
            }   
        }
        close(fd_in_);
        if(fd_out_ >= 0) {
            close(fd_out_);
        }
        log().info("~connection");
    }
    int
    connection::connect() 
    {
        struct sockaddr_in address = {0};
        address.sin_family = AF_INET;
        address.sin_port = htons(port_out_);
        int res = 0;
        struct addrinfo* addr = nullptr;
        struct addrinfo hints = { 0 };
        hints.ai_flags = 1024;
        hints.ai_family = PF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        res = getaddrinfo(addr_out_.c_str(), nullptr, &hints, &addr);

        if(!res)
        {
            memcpy(&address.sin_addr, &((struct sockaddr_in*) addr->ai_addr)->sin_addr, sizeof(struct in_addr));
            freeaddrinfo(addr);
        }
        else
        {
            // unable to resolve hostname
            if(inet_pton(PF_INET, addr_out_.c_str(), &address.sin_addr) != 1)
            {
                log().error("Couldn't connect to {}", addr_out_);
                return -1; 
            }
        }
        fd_out_ = socket(PF_INET, SOCK_STREAM, 0);
        if(fd_out_ < 0) {
            log().error("socket: {}", strerror(errno));
            return -errno;
        }

        if(::connect(fd_out_, (struct sockaddr*)&address, sizeof(address))) {
            log().error("connect: {}", strerror(errno));
            close(fd_out_);
            fd_out_ = -1;
            return -errno;
        }
        int flags = fcntl(fd_out_, F_GETFL, 0);
        fcntl(fd_out_, F_SETFL, flags | O_NONBLOCK);
        flags = fcntl(fd_in_, F_GETFL, 0);
        fcntl(fd_in_, F_SETFL, flags | O_NONBLOCK);

        register_handler_(fd_in_, [this](int fd, uint32_t events) { return handle_events(fd, events); });
        register_handler_(fd_out_, [this](int fd, uint32_t events) { return handle_events(fd, events); });
        log().info("Connected to {}:{}", addr_out_, port_out_);

        res = pipe(&fd_pipe_[0]);
        if(!res) {
            pipe_max_size_ = fcntl(fd_pipe_[0], F_GETPIPE_SZ);  
            res = pipe(&fd_pipe_[2]);
        }
        return res;
    }

    int
    connection::handle_events(int fd, uint32_t events)
    {
        int res = 0;
        const bool is_in = (fd == fd_in_);
        const int other = is_in ? fd_out_ : fd_in_;
        const int* fd_pipe = &fd_pipe_[2 * !is_in];
        unsigned int& pipe_size = is_in ? pipe_size_[0] : pipe_size_[1];

        if(events & EPOLLIN) {
            // Got some data.

            do {
                res = splice(fd, NULL, fd_pipe[1], NULL, pipe_max_size_ - pipe_size, SPLICE_F_MOVE | SPLICE_F_NONBLOCK | SPLICE_F_MORE);
                if(res <= 0) break;
                pipe_size += res;
                while(pipe_size > 0) {
                    res = splice(fd_pipe[0], NULL, other, NULL, pipe_size, SPLICE_F_MOVE | SPLICE_F_NONBLOCK | SPLICE_F_MORE);    
                    if(res <= 0) break;
                    pipe_size -= res;
                }
                
            } while(res > 0);

            if(res == -1 && errno != EWOULDBLOCK) {
                log().error("splice: {}", strerror(errno));
                res = -errno;
                end_handler_(this);
            } else if (res == 0) { 
                // Got 0, connection has ended.
                end_handler_(this);
            } else {
                res = 0;
            }
        }
        if(events & EPOLLOUT) {
            while(pipe_size > 0) {
                res = splice(fd_pipe[0], NULL, other, NULL, pipe_size, SPLICE_F_MOVE | SPLICE_F_NONBLOCK | SPLICE_F_MORE);    
                if(res <= 0) break;
                pipe_size -= res;
            }

            if(res == -1 && errno != EWOULDBLOCK) {
                log().error("splice: {}", strerror(errno));
                res = -errno;
                end_handler_(this);
            } else {
                res = 0;
            }
        }
        
        return res;
    }
}
}