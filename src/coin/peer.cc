#include <skycoin/coin/peer.hh>
#include <skycoin/coin/coder.hh>
#include <skycoin/uri.hh>

#include <arpa/inet.h>
#include <array>

namespace skycoin { namespace coin {
    
    size_t
    peer::serialize(std::vector<uint8_t>& data) 
    {
        encoder e(data);

        size_t res = 0;

        std::array<uint8_t, 6> ar;
        uint32_t* addr = reinterpret_cast<uint32_t*>(&ar[0]);
        uint16_t* port = reinterpret_cast<uint16_t*>(&ar[4]);
        {
            uri u(address);
            // TODO: (maybe) try getaddrinfo first, IPv6 compatibility (but this has to wait until the Skycoin
            // protocol supports it.)
            inet_pton(PF_INET, u.host.c_str(), (void*)addr);
            *addr = ntohl(*addr);
            *port = uint16_t(u.port);
        }
        res += e.set(ar);

        return res;
    }

    size_t
    peer::deserialize(uint8_t* data, size_t size) 
    {
        decoder d(data, size);
        uint32_t addr = 0;
        uint16_t port = 0;

        d.safe_get(addr);
        d.safe_get(port);
        addr = htonl(addr);
        {
            char a[INET6_ADDRSTRLEN]{};
            inet_ntop(PF_INET, &addr, a, sizeof(a));
            address = "tcp://" + std::string(a) + ":" + std::to_string(port);
        }
        
        return d.p() - data;
    }

    size_t
    get_peers(uint8_t* data, size_t size, std::vector<peer>& peers)
    {
        decoder d(data, size);
        uint32_t count = 0;
        d.safe_get(count);
        for(uint32_t i = 0; i < count; i++) {
            peer p;
            size_t res = p.deserialize(d.p(), d.remain());
            if(res > 0) {
                peers.push_back(p);
                d.advance(res);
            } else {
                break;
            }
        }
        return d.p() - data;
    }
    size_t 
    set_peers(std::vector<peer>& peers, std::vector<uint8_t>& out_data)
    {
        size_t res = 0;
        encoder e(out_data);
        uint32_t count(peers.size());
        res = e.set(count);
        for(auto& it : peers) {
            res += it.serialize(out_data);
        }
        return res;
    }
}
}