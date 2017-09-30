#include <skycoin/coin/block.hh>
#include <skycoin/coin/coder.hh>
#include <skycoin/log.hh>

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
        
        decoder d(data, size);

        d.safe_get(length);
        d.safe_get(type);
        d.safe_get(inner_hash);
        d.safe_get(signatures);
        d.safe_get(outputs_spent);

        uint32_t tx_outputs = 0;
        d.safe_get(tx_outputs);

        for(uint32_t i = 0 ; i < tx_outputs; i++) {
            transaction_output to;
            d.safe_get(to.address);
            d.safe_get(to.coins);
            d.safe_get(to.hours);
            outputs.push_back(to);
        }

        return d.p() - data;
    }

    size_t
    block::serialize(std::vector<uint8_t>& data) {
        size_t res = 0;

        return res;
    }

    size_t
    block::deserialize(uint8_t* data, size_t size) {
        decoder d(data, size);

        d.safe_get(version);
        d.safe_get(time);
        d.safe_get(sequence);
        d.safe_get(fee);
        d.safe_get(prev_hash);
        d.safe_get(body_hash);
        d.safe_get(unspent_hash);

        uint32_t tx_count = 0;
        d.safe_get(tx_count);

        for(uint32_t i = 0 ; i < tx_count; i++) {
            transaction t;
            auto res = t.deserialize(d.p(), d.remain());
            if(res > 0) { 
                transactions.emplace_back(std::move(t));
                d.advance(res);
            } else {
                // error.
                log().error("Error deseiralizing transactions.");
                break;
            }
        }

        return d.p() - data;
    }
}
}