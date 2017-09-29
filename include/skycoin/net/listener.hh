#ifndef SKYCOIN__LISTENER_HH
#define SKYCOIN__LISTENER_HH

#include <skycoin/net/connection.hh>

namespace skycoin {


    // Interface for listeners
    // A listener will listen to a particular port
    // and as new connections are established start them up and 
    // let its owner know that it has a new connection.

    // Note that several of these functions are also used in i_connection.
    // That is because the listener should automatically set the equivalent
    // functions in the new connections as they are created.


    class i_listener;

    using new_connection_handler_f = std::function<void(i_connection&)>;

    class i_listener : public event_handler {
    public:
        virtual ~i_listener() {};
        
        void set_can_read_handler(can_read_handler_f handler) { can_read_handler_ = handler; }; 
        void set_can_write_handler(can_write_handler_f handler) { can_write_handler_ = handler; };
        void set_new_connection_handler(new_connection_handler_f handler) { new_connection_handler_ = handler; };
        void set_end_handler(end_handler_f handler) { end_handler_ = handler; };
        
        virtual void close(int fd) = 0;
        virtual int fd() const = 0;
        virtual i_connection* connection_for_fd(int fd) = 0;
        
    protected:
        end_handler_f end_handler_;
        new_connection_handler_f new_connection_handler_;
        can_read_handler_f can_read_handler_;
        can_write_handler_f can_write_handler_;
    };

}

#endif