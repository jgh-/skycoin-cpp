#include <skycoin/net/tcp/listener.hh>
#include <skycoin/coin/client.hh>
#include <skycoin/uri.hh>
#include <skycoin/log.hh>

namespace skycoin { namespace coin {

    client::client(event_loop& el, unpause::async::thread_pool& pool, std::string configuration)
    : config_(nlohmann::json::parse(configuration))
    , pool_(pool)
    , event_loop_(el) {}

    
    int
    client::start()
    {
        int res = 0;

        // start listener
        {
            uri listener_uri(config_.at("listener").get<std::string>());

            // Create the listener based on the URI.  Choose the
            // correct one based on uri scheme.
            // Note: Currently skycoin only seems to support TCP, though Skywire is a UDP-based protocol so
            //       I will implement that as Skywire's protocol is built out.
            if(listener_uri.scheme == "tcp") {
                listener_ = std::make_unique<tcp::listener>(listener_uri.host, listener_uri.port, pool_);
            }


            // 
            // Here we have our listener, and we want to set our connection callbacks with the listener
            // which will own incoming connections.  We will own any outgoing connections we want to create.
            // 
            if(listener_) {
                listener_->set_register_handler([this](int fd, skycoin::event_handler_f handler) {
                    this->event_loop_.register_handler(fd, handler);
                });

                listener_->set_unregister_handler([this](int fd) {
                    this->event_loop_.unregister_handler(fd);
                });
                
                listener_->set_new_connection_handler([this](i_connection& conn) {
                    new_incoming_connection(conn);
                });

                listener_->set_can_read_handler([this](i_connection& c) {
                    uint8_t buf[1024]{};
                    auto res = c.read(buf, sizeof(buf));
                    log().info("got some stuff: {} ", buf);
                    return res;
                });
                
                listener_->set_can_write_handler([this](i_connection& c) {
                    return -1;
                });

                listener_->set_end_handler([this](i_connection* c, int32_t error) {

                });

                event_loop_.register_handler(listener_->fd(), listener_->handler());
            }
        }


        // Configure blockchain
        {

        }

        // Connect to default peers
        {

        }
        return res;
    }
    int
    client::stop(bool graceful)
    {
        int res = 0;
        
        if(listener_) {
            // Unregister listener so it stops taking new connections
            event_loop_.unregister_handler(listener_->fd());
        }
        
        if(!graceful) {
            // Drop all existing connections
            listener_.reset();
        }
        return res;
    }

    void
    client::new_incoming_connection(i_connection& conn)
    {
        
    }
}
}