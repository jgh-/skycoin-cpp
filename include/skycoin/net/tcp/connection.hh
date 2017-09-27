
#ifndef SKYCOIN__TCP_CONNECTION_HH
#define SKYCOIN__TCP_CONNECTION_HH

#include <skycoin/net/connection.hh>
#include <string>

namespace skycoin { namespace tcp {

    class connection : public i_connection
    {
    public:

        connection(int fd);
        connection(std::string addr, int port);
        ~connection();

        event_handler_f handler() { return [this](int fd, uint32_t events) {
            return handle_events(fd, events);
        }; };

        virtual int fd() const { return fd_; }
        virtual int connect();
        virtual ssize_t read(uint8_t* buf, size_t size);
        virtual ssize_t write(uint8_t* buf, size_t size);

    protected:
        virtual int read_event(int fd);
        virtual int write_event(int fd);
        virtual int handle_events(int fd, uint32_t events);
        
    protected:
        
        std::string addr_;
        int port_;
        int fd_;
    };  
}
}
#endif