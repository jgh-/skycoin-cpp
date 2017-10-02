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

        encoder e(data);

        auto length_pos = data.size(); // save this for later.
        res += e.set(length);
        res += e.set(type);
        res += e.set(inner_hash);
        res += e.set(signatures);
        res += e.set(outputs_spent);

        uint32_t tx_outputs(outputs.size());
        res += e.set(tx_outputs);
        for(auto& it : outputs) {
            res += e.set(it.address);
            res += e.set(it.coins);
            res += e.set(it.hours);
        }
        uint32_t* len = reinterpret_cast<uint32_t*>(&data[length_pos]);
        *len = res; // length here appears to be inclusive.

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
        encoder e(data);
        res += e.set(version);
        res += e.set(time);
        res += e.set(sequence);
        res += e.set(fee);
        res += e.set(prev_hash);
        res += e.set(body_hash);
        res += e.set(unspent_hash);
        uint32_t tx_count(transactions.size());
        res += e.set(tx_count);
        for(auto& it : transactions) {
            res += it.serialize(data);
        }
        res += e.set(signature);
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
        d.safe_get(signature);

        return d.p() - data;
    }

    size_t
    get_blocks(uint8_t* data, size_t size, std::vector<block>& out_blocks) {
        decoder d(data, size);
        
        uint32_t count = 0;
        d.safe_get(count);

        for(uint32_t i = 0 ; i < count ; i++) {
            block b;
            auto res = b.deserialize(d.p(), d.remain());
            if(res > 0) {
                d.advance(res);
                out_blocks.emplace_back(std::move(b));
            } else { 
                break;
            }
        }
        return d.p() - data;
    }

    size_t
    set_blocks(std::vector<block>& blocks, std::vector<uint8_t>& out_data) {
        size_t res = 0;
        uint32_t count = static_cast<uint32_t>(blocks.size());
        encoder e(out_data);
        res += e.set(count);
        for(auto& block : blocks) {
            res += block.serialize(out_data);
        }
        return res;
    }
}
}