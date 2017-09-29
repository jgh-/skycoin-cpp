
#include <skycoin/net/tcp/listener.hh>
#include <skycoin/log.hh>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/eventfd.h>
#include <sys/prctl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>
#include <fcntl.h>

namespace skycoin { namespace tcp {

    listener::listener(std::string in_addr, int in_port, unpause::async::thread_pool& pool) 
    : addr_(in_addr)
    , pool_(pool)
    , fd_(-1)
    , port_(in_port)
    {
        struct addrinfo hints {};
        struct addrinfo *l_addr = nullptr;

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        const char* host = in_addr == "*" ? nullptr : in_addr.c_str();

        int res = getaddrinfo(host, std::to_string(in_port).c_str(), &hints, &l_addr);
        
        if(res < 0) {
            log().error("getaddrinfo {}", gai_strerror(res));
            return;
        }

        fd_ = socket(l_addr->ai_family, l_addr->ai_socktype, l_addr->ai_protocol);
        if(fd_ < 0)
        {
            log().error("Unable to open port {}: {}", in_port, strerror(errno));
            freeaddrinfo(l_addr);
            return;
        }
        
        res = fcntl (fd_, F_GETFL, 0);

        if(res >= 0)
        {
            res |= O_NONBLOCK;
            res = fcntl (fd_, F_SETFL, res);
        }
        
        if(res == -1)
        {
            log().error("fcntl: {}", strerror(errno));
            close(fd_);
            fd_ = -1;
            freeaddrinfo(l_addr);
            return;
        }

        res = bind(fd_, l_addr->ai_addr, l_addr->ai_addrlen);

        freeaddrinfo(l_addr);

        if(fd_ > -1 && !res)
        {
            res = listen(fd_, SOMAXCONN);
            if(res < 0)
            {
                log().error("Listen: {}", strerror(errno));
                close(fd_);
                fd_ = -1;
            }
        }
        else
        {
            log().error("Bind: {}", strerror(errno));
            close(fd_);
            fd_ = -1;
        }
    }

    listener::~listener() {
        if(fd_ > -1) {
            close(fd_);
        }
    }

    int 
    listener::handle_events(uint32_t events)
    {        
        int res = -1 * ((events & EPOLLERR) || (events & EPOLLHUP));

        struct sockaddr addr = {0};
        socklen_t len = 0;
        int sd = 0;
        
        while(!res) {
            len = sizeof(in_addr);
            sd = accept(fd_, &addr, &len);
            if(sd == -1)
            {
                if(errno == EWOULDBLOCK)
                {
                    break;
                }
                else
                {
                    log().error("Accept: {}", strerror(errno));
                    res = -1;
                }
            }
            else
            {
                // new connection
                log().info("New connection on port {}", port_);
                handle_new_connection(sd);
            }
        }

        return res;
    }

    void
    listener::handle_new_connection(int fd) {
        std::unique_ptr<connection> c  = std::make_unique<connection>(fd);
        c->set_can_read_handler(can_read_handler_);
        c->set_can_write_handler(can_write_handler_);
        c->set_register_handler(register_handler_);
        c->set_unregister_handler(unregister_handler_);
        c->set_end_handler([this](i_connection* conn, int32_t error){

            unpause::async::run(pool_, 
                [this, conn, error] { 
                    if(end_handler_) {
                        end_handler_(conn, error);
                    }
                    for(auto it = connections_.begin(); it != connections_.end(); ++it) {
                        if(it->get() == conn) {
                            connections_.erase(it);
                            break;
                        }
                    }
                });
        });

        if(!c->connect()) {
            if(new_connection_handler_) {
                new_connection_handler_(*c);
            }
            connections_.push_back(std::move(c));
        } else {
            log().error("Couldn't connect, dropping connection");
        }
    }
}
}