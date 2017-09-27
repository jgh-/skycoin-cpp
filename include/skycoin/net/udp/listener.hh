#ifndef SKYCOIN__UDP_LISTENER_HH
#define SKYCOIN__UDP_LISTENER_HH

#include <skycoin/net/udp/connection.hh>
#include <unpause/async>
#include <memory>
#include <list>

namespace skycoin { namespace udp {

    class listener : public event_handler {
    public:
        listener(std::string addr, int port, unpause::async::thread_pool& pool);
        ~listener();
        
        event_handler_f handler() { return [this](int fd, uint32_t events) { return handle_events(events); }; }

        int fd() const { return fd_; }

        void set_can_read_handler(can_read_handler_f handler) { can_read_handler_ = handler; };
        void set_can_write_handler(can_write_handler_f handler) { can_write_handler_ = handler; };

    protected:
        virtual int handle_events(uint32_t events);
        virtual void handle_new_connection(int fd);

    protected:
        can_read_handler_f can_read_handler_;
        can_write_handler_f can_write_handler_;
        
        std::list<std::unique_ptr<connection>> connections_;
        std::string addr_;
        unpause::async::thread_pool& pool_;

        int fd_;
        int port_;

    };

} // namespace tcp
} // namespace skycoin

#endif // skycoin__UDP_LISTENER_HH