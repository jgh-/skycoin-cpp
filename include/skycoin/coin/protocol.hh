#ifndef SKYCOIN__COIN_PROTOCOL_HH
#define SKYCOIN__COIN_PROTOCOL_HH

#include <skycoin/net/listener.hh>
#include <skycoin/event_loop.hh>

namespace skycoin { namespace coin {

    class protocol {
    public:

        protocol(event_loop& el, std::string configuration);
        ~protocol() {};

    private:

        std::unique_ptr<i_listener> listener;
    }
}
}

#endif