#include <skycoin/net/udp/listener.hh>
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

namespace skycoin { namespace udp {

    listener::listener(std::string addr, int port, unpause::async::thread_pool& pool)
    : addr_(addr)
    , pool_(pool)
    , fd_(-1)
    , port_(port)
    {

        struct addrinfo hints {};
        struct addrinfo *l_addr = nullptr;

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE;

        const char* host = addr_ == "*" ? nullptr : addr_.c_str();

        int res = getaddrinfo(host, std::to_string(port_).c_str(), &hints, &l_addr);

        if(res < 0) {
            log().error("getaddrinfo {}", gai_strerror(res));
            return;
        }

        fd_ = socket(l_addr->ai_family, l_addr->ai_socktype, l_addr->ai_protocol);
        if(fd_ < 0)
        {
            log().error("Unable to open port {}: {}", port_, strerror(errno));
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

        if(bind(fd_, l_addr->ai_addr, l_addr->ai_addrlen)) {
            log().error("Unable to bind: {}", strerror(errno));
            close(fd_);
            fd_ = -1;
            freeaddrinfo(l_addr);
            return;
        }
        freeaddrinfo(l_addr);
    }

    listener::~listener() 
    {
        if(fd_ > -1) {
            close(fd_);
        }
    }

    int 
    listener::handle_events(uint32_t events)
    {
        log().info("event: {}", events);
        int res = -1 * ((events & EPOLLERR) || (events & EPOLLHUP));

        struct sockaddr_storage addr {};
        while(!res) {
            
            if(events & EPOLLIN) {
                uint8_t buf[1024] {};
                socklen_t fromlen = sizeof(addr);
                res = recvfrom(fd_, buf, sizeof(buf), 0, (struct sockaddr*)&addr, &fromlen);
                log().info("Got {}", buf);
            } else {
                res = -1;
            }
        }
        return 0;
    }
}
}