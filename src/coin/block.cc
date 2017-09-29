#include <skycoin/coin/block.hh>
#include <skycoin/coin/coder.hh>

namespace skycoin { namespace coin {
    
    template<typename T>
    static void safe_get(T& val, decoder& d) {
        auto r = d.get<T>();
        if(r) {
            val = *r;
        }
    }

    // Transactions and blocks are stored using the mechanisms described in "coder.hh"
    size_t
    transaction::serialize(std::vector<uint8_t>& data) {
        size_t res = 0;

        return res;
    }

    size_t
    transaction::deserialize(uint8_t* data, size_t size) {
        size_t res = 0;

        return res;
    }

    size_t
    block::serialize(std::vector<uint8_t>& data) {
        size_t res = 0;

        return res;
    }

    size_t
    block::deserialize(uint8_t* data, size_t size) {
        size_t res = 0;
        decoder d(data, size);

        d.safe_get(version);
        d.safe_get(time);
        d.safe_get(sequence);
        d.safe_get(fee);
        d.safe_get(prev_hash);
        d.safe_get(body_hash);
        d.safe_get(unspent_hash);

        uint32_t count;
        
        return res;
    }
}
}