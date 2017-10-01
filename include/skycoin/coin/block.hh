#ifndef SKYCOIN__COIN_BLOCK_HH
#define SKYCOIN__COIN_BLOCK_HH

#include <vector>
#include <array>

namespace skycoin { namespace coin {

    using sha256_t = std::array<uint8_t, 32>;
    using signature_t = std::array<uint8_t, 65>;
  

  // From the golang code:

/*
Addresses are the Ripemd160 of the double SHA256 of the public key
- public key must be in compressed format

In the block chain the address is 20+1 bytes
- the first byte is the version byte
- the next twenty bytes are RIPMD160(SHA256(SHA256(pubkey)))

In base 58 format the address is 20+1+4 bytes
- the first 20 bytes are RIPMD160(SHA256(SHA256(pubkey))).
-- this is to allow for any prefix in vanity addresses
- the next byte is the version byte
- the next 4 bytes are a checksum
-- the first 4 bytes of the SHA256 of the 21 bytes that come before

*/
    using address_t = std::array<uint8_t, 21>; // 1-byte Version, 20-byte key.


    struct transaction {
        
        struct transaction_output {
            address_t address;
            uint64_t  coins;
            uint64_t  hours;
        };

        size_t serialize(std::vector<uint8_t>& data);
        size_t deserialize(uint8_t* data, size_t size);
        
        sha256_t inner_hash;
        
        std::vector<signature_t> signatures;
        std::vector<sha256_t> outputs_spent;
        std::vector<transaction_output> outputs;

        uint32_t length;
        uint8_t  type;
    };

    struct block {

        size_t serialize(std::vector<uint8_t>& data);
        size_t deserialize(uint8_t* data, size_t size);

        signature_t signature;

        sha256_t prev_hash;
        sha256_t body_hash;
        sha256_t unspent_hash;

        uint64_t time;
        uint64_t sequence;
        uint64_t fee;

        uint32_t version;

        std::vector<transaction> transactions;
    };

    size_t get_blocks(uint8_t* data, size_t size,  std::vector<block>& out_blocks);
    size_t set_blocks(std::vector<block>& blocks, std::vector<uint8_t>& out_data);
    
}
}

#endif