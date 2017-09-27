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
    connection::connection(int fd)
    : port_(0)
    , fd_(fd)
    {
    }
    connection::connection(std::string addr, int port)
    : addr_(addr)
    , port_(port)
    , fd_(-1)
    {}

    connection::~connection() {
        if(unregister_handler_) {
            unregister_handler_(fd_);
        }
        log().info("~connection");
    }

    int
    connection::connect() 
    {
        int res = 0;

        if(fd_ == -1 && addr_.size() && port_) {
            struct sockaddr_in address = {0};
            address.sin_family = AF_INET;
            address.sin_port = htons(port_);
            
            struct addrinfo* addr = nullptr;
            struct addrinfo hints = { 0 };
            hints.ai_flags = 1024;
            hints.ai_family = PF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            res = getaddrinfo(addr_.c_str(), nullptr, &hints, &addr);

            if(!res)
            {
                memcpy(&address.sin_addr, &((struct sockaddr_in*) addr->ai_addr)->sin_addr, sizeof(struct in_addr));
                freeaddrinfo(addr);
            }
            else
            {
                // unable to resolve hostname
                if(inet_pton(PF_INET, addr_.c_str(), &address.sin_addr) != 1)
                {
                    log().error("Couldn't connect to {}", addr_);
                    return -1; 
                }
            }
            fd_ = socket(PF_INET, SOCK_STREAM, 0);
            if(fd_ < 0) {
                log().error("socket: {}", strerror(errno));
                return -errno;
            }

            if(::connect(fd_, (struct sockaddr*)&address, sizeof(address))) {
                log().error("connect: {}", strerror(errno));
                close(fd_);
                fd_ = -1;
                return -errno;
            }
            log().info("Connected to {}:{}", addr_, port_);
        }
        
        if(fd_ > -1) {
            int flags = fcntl(fd_, F_GETFL, 0);
            res = fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
            if(register_handler_) {
                register_handler_(fd_, [this](int fd, uint32_t events) { return handle_events(fd, events); });
            }
        }

        return res;
    }

    int
    connection::handle_events(int fd, uint32_t events)
    {
        int res = 0;

        if(events & EPOLLIN) {
            res = read_event(fd);
        }
        if(events & EPOLLOUT) {
            res = write_event(fd);
        }
        
        return res;
    }

    int
    connection::read_event(int fd)
    {
        int res = 0;

        uint8_t buffer[8] = {0};
        ssize_t r = recv(fd_, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT);

        if(r > 0 && can_read_handler_) {
            res = can_read_handler_(*this);
        } else if(!r) {
            end_handler_(this, 0);
        }

        return res;
    }

    int
    connection::write_event(int fd)
    {
        int res = 0;

        if(can_write_handler_) {
            res = can_write_handler_(*this);
        }

        return res;
    }

    ssize_t
    connection::write(uint8_t* data, size_t size)
    {
        return ::write(fd_, data, size);
    }

    ssize_t
    connection::read(uint8_t* data, size_t size) 
    {
        return ::read(fd_, data, size);
    }
}
}