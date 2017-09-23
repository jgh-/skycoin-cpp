#ifndef SKYWIRE__UDP_LISTENER_HH
#define SKYWIRE__UDP_LISTENER_HH

#include <skywire/net/udp/connection.hh>
#include <unpause/async>
#include <memory>
#include <list>

namespace skywire { namespace udp {

    class listener : public event_handler {
    public:
        // temporarily have an out address,
        // this will eventually be removed and we'll use more intelligent routing.
        listener(std::string addr, int in_port, std::string out_addr, int out_port, unpause::async::thread_pool& pool);

        event_handler_f handler() { return [this](int fd, uint32_t events) { return handle_events(events); }; }

        int fd() const { return fd_; }

    private:
        int handle_events(uint32_t events);

    private:
        std::list<std::unique_ptr<connection>> connections_;
        std::string addr_in_;
        std::string addr_out_;
        unpause::async::thread_pool& pool_;

        int fd_;
        int port_in_;
        int port_out_;

    };

} // namespace tcp
} // namespace skywire

#endif // SKYWIRE__UDP_LISTENER_HH