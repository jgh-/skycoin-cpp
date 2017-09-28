#include <skycoin/net/tcp/listener.hh>
#include <skycoin/coin/client.hh>
#include <skycoin/uri.hh>
#include <skycoin/log.hh>

namespace skycoin { namespace coin {

    client::client(event_loop& el, unpause::async::thread_pool& pool, std::string configuration)
    : config_(nlohmann::json::parse(configuration))
    , event_loop_(el)
    , pool_(pool)
    {
    }

    int
    client::start()
    {
        int res = 0;

        // start listener
        {
            uri listener_uri(config_.at("listener").get<std::string>());
            if(listener_uri.scheme == "tcp") {
                listener_ = std::make_unique<tcp::listener>(listener_uri.host, listener_uri.port, pool_);
            }

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
                    log().info("size: {} ", res);
                    return res;
                });

                listener_->set_end_handler([this](i_connection* c, int32_t error) {
                    if(c) {
                        event_loop_.unregister_handler(c->fd());
                    }
                });

                event_loop_.register_handler(listener_->fd(), listener_->handler());
            }
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