#ifndef SKYCOIN__CONNECTION_HH
#define SKYCOIN__CONNECTION_HH

#include <skycoin/event_handler.hh>

namespace skycoin {

    class i_connection;
    
    using can_read_handler_f = std::function<int(i_connection&)>; // data available for reading
    using can_write_handler_f = std::function<int(i_connection&)>;  // space available for writing
    using end_handler_f = std::function<void(i_connection*, int32_t error)>;

    class i_connection : public event_handler {
    public:
        virtual ~i_connection() {};
        virtual ssize_t read(uint8_t* buffer, size_t size) = 0;
        virtual ssize_t write(uint8_t* buffer, size_t size) = 0;
        virtual int connect() = 0;
        
        void set_can_read_handler(can_read_handler_f handler) { can_read_handler_ = handler; };
        void set_can_write_handler(can_write_handler_f handler) { can_write_handler_ = handler; };
        void set_end_handler(end_handler_f handler) { end_handler_ = handler; };
    protected:
        end_handler_f           end_handler_;
        can_read_handler_f      can_read_handler_;
        can_write_handler_f     can_write_handler_;
    };
}

#endif