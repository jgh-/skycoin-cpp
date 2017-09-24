
#ifndef skycoin__UDP_CONNECTION_HH
#define skycoin__UDP_CONNECTION_HH

#include <skycoin/event_handler.hh>
#include <string>

namespace skycoin { namespace udp {

    class connection;

    using end_handler_f = std::function<void(connection*)>;

    class connection : public event_handler
    {
    public:

        connection(int fd, std::string local_addr, int local_port);
        ~connection();

        event_handler_f handler() { return [this](int fd, uint32_t events) {
            return handle_events(fd, events);
        }; };

        int fd() const { return fd_in_; }
        int connect();
        void set_end_handler(end_handler_f handler) { end_handler_ = handler; };

    private:
        int handle_events(int fd, uint32_t events);
        
    private:
        end_handler_f end_handler_;
        std::string addr_out_;
        int fd_pipe_[4];
        unsigned int pipe_size_[2];
        int pipe_max_size_;
        int port_out_;
        int fd_in_;
        int fd_out_;
    };  
}
}
#endif