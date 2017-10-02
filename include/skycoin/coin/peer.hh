#ifndef SKYCOIN__COIN_PEER_HH
#define SKYCOIN__COIN_PEER_HH

#include <stdint.h>
#include <vector>
#include <string>

namespace skycoin { namespace coin {

    struct peer {
        size_t serialize(std::vector<uint8_t>& data);
        size_t deserialize(uint8_t* data, size_t size);

        std::string address;
        uint64_t    last_seen;
        int32_t     retry_times;
        bool        is_private;
        bool        is_trusted;
        bool        has_incoming_port;

    };
    
    size_t get_peers(uint8_t* data, size_t size, std::vector<peer>& peers);
    size_t set_peers(std::vector<peer>& peers, std::vector<uint8_t>& out_data);
}
}

#endif