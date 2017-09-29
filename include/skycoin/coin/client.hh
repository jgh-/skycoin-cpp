#ifndef SKYCOIN__COIN_CLIENT_HH
#define SKYCOIN__COIN_CLIENT_HH

#include <skycoin/net/listener.hh>
#include <skycoin/event_loop.hh>

#include <json/json.hpp>
#include <unpause/async>
#include <memory>

namespace skycoin { namespace coin {

    class client {
    public:

        client(event_loop& el, unpause::async::thread_pool& pool, std::string configuration); 

        // Returns 0 if no error.
        int start();
        int stop(bool graceful);

    private:
        void new_incoming_connection(i_connection& conn);
        
    private:
        nlohmann::json config_;
        unpause::async::task_queue   queue_;
        unpause::async::thread_pool& pool_;
        event_loop& event_loop_;

        std::unique_ptr<i_listener> listener_;
    };
}
}

#endif