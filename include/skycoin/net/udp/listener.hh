#ifndef SKYCOIN__UDP_LISTENER_HH
#define SKYCOIN__UDP_LISTENER_HH

#include <skycoin/net/udp/connection.hh>
#include <skycoin/net/listener.hh>
#include <unpause/async>
#include <memory>
#include <list>

namespace skycoin { namespace udp {

    class listener : public i_listener {
    public:
        listener(std::string addr, int port, unpause::async::thread_pool& pool);
        ~listener();
        
        event_handler_f handler() { return [this](int fd, uint32_t events) { return handle_events(events); }; }

        virtual void close(int fd) { 
            for(auto it = connections_.begin(); it != connections_.end();) {
                if((*it)->fd() == fd) {
                    it = connections_.erase(it);
                } else {
                    ++it;
                }
            }
        };
        int fd() const { return fd_; }

    protected:
        virtual int handle_events(uint32_t events);
        virtual void handle_new_connection(int fd);

    protected:
        std::list<std::unique_ptr<connection>> connections_;
        std::string addr_;
        unpause::async::thread_pool& pool_;

        int fd_;
        int port_;

    };

} // namespace tcp
} // namespace skycoin

#endif // skycoin__UDP_LISTENER_HH