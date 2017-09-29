#ifndef SKYCOIN__COIN_BLOCK_HH
#define SKYCOIN__COIN_BLOCK_HH

#include <vector>

namespace skycoin { namespace coin {

    using sha256_t = uint8_t[32];
    using signature_t = uint8_t[65];
    using address_t = uint8_t[25]; // 1-byte Version, 20-byte key, 4-byte checksum

    struct transaction_output {
        address_t address;
        uint64_t  coins;
        uint64_t  hours;
    };

    struct transaction {
        
        size_t serialize(std::vector<uint8_t>& data);
        size_t deserialize(uint8_t* data, size_t size);

        sha256_t inner_hash;
        
        std::vector<signature_t> signatures;
        std::vector<sha256_t> outputs_spent;
        std::vector<transaction_output> outputs_created;

        uint32_t length;
        uint8_t  type;
    };

    struct block {

        size_t serialize(std::vector<uint8_t>& data);
        size_t deserialize(uint8_t* data, size_t size);
        
        sha256_t prev_hash;
        sha256_t body_hash;
        sha256_t unspent_hash;

        uint64_t time;
        uint64_t sequence;
        uint64_t fee;

        uint32_t version;

        std::vector<transaction> transactions;
    };

}
}

#endif