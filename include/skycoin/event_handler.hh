#ifndef SKYCOIN__EVENT_HANDLER_HH
#define SKYCOIN__EVENT_HANDLER_HH

#include <functional>

namespace skycoin {

    using event_handler_f = std::function<int(int fd, uint32_t events)>;
    using register_handler_f = std::function<void(int, event_handler_f)>;
    using unregister_handler_f = std::function<void(int)>;
    
    class event_handler {
    public:
        virtual ~event_handler() {};
        virtual event_handler_f handler() = 0;
        virtual int fd() const = 0;

        void set_register_handler(register_handler_f register_handler) {
            register_handler_ = [this, register_handler] (int fd, const event_handler_f& f) 
                                        { registered_ = true; register_handler(fd, f); };
        }
        
        void set_unregister_handler(unregister_handler_f unregister_handler) {
            unregister_handler_ = [this, unregister_handler] (int fd) 
                                        { registered_ = false; unregister_handler(fd); };
        }

        bool registered() const { return registered_; };

    protected:
        register_handler_f      register_handler_;
        unregister_handler_f    unregister_handler_;
        bool registered_ = false;
    };  // class event_handler
}   // namespace skycoin

#endif // skycoin__EVENT_LOOP_HH