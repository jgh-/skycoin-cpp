#ifndef SKYCOIN__COIN_MESSAGE_HH
#define SKYCOIN__COIN_MESSAGE_HH

#include <skycoin/coin/peer.hh>
#include <skycoin/coin/block.hh>

#include <string>
#include <memory>
#include <array>

#include <stdint.h>

namespace skycoin { namespace coin {


    // FourCharCode handling for the Int32 names messages have
    constexpr int32_t fourcc( const char p[5] )
    {
        return (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
    }

    namespace detail {        
        constexpr void fourcc_str(int32_t in, char out[5])
        {
            out[4] = 0;
            out[3] = (in >> 24) & 0xff;
            out[2] = (in >> 16) & 0xff;
            out[1] = (in >> 8) & 0xff;
            out[0] = in & 0xff;
        }
    }

    struct fourcc_t {
        fourcc_t(const char n[5]) : name(fourcc(n)) {};
        fourcc_t(int32_t n) : name(n) {};
        fourcc_t() : name(0) {};
        
        std::string str() const {
            char p[5] = {0};
            detail::fourcc_str(name, p);
            return p;
        };
        
        int32_t operator()() { return name; };
        
        friend bool operator<(const fourcc_t& lhs, const fourcc_t& rhs) { return lhs.name < rhs.name; };
        friend bool operator==(const fourcc_t& lhs, const int32_t& rhs) { return lhs.name == rhs; };
        friend bool operator==(const fourcc_t& lhs, const fourcc_t& rhs) { return lhs.name == rhs.name; };
        int32_t name;
    };

    // ------------------------------------------------------------

    enum message_t {
        MsgIntro = fourcc("INTR"),
        MsgGetBlocks = fourcc("GETB"),
        MsgGiveBlocks = fourcc("GIVB"),
        MsgPing = fourcc("PING"),
        MsgPong = fourcc("PONG"),
        MsgGetPeers = fourcc("GETP"),
        MsgGivePeers = fourcc("GIVP"),
        MsgAnnounceBlocks = fourcc("ANNB"),
        MsgGetTxns = fourcc("GETT"),
        MsgGiveTxns = fourcc("GIVT"),
        MsgANnounceTxns = fourcc("ANNT")
    };


    struct message_base 
    {
        virtual size_t deserialize(uint8_t* data, size_t len) {
            if(len>=8) {
                size = *reinterpret_cast<uint32_t*>(data);
                name = *reinterpret_cast<uint32_t*>(data+4);
                return 8;
            }
            return 0;
        };
        virtual size_t serialize(std::vector<uint8_t>& data) {
            data.insert(data.end(), (uint8_t*)&size, ((uint8_t*)&size)+4);
            data.insert(data.end(), (uint8_t*)&name.name, ((uint8_t*)&name.name)+4);
            return 8;
        };

        uint32_t size;       // Size does not appear to be inclusive of the size field,
        fourcc_t name;       // but is inclusive of the name.
    };
    
    template<int32_t Name>
    struct message : public message_base
    {
        size_t deserialize(uint8_t* data, size_t size) { return 0 ; };
        size_t serialize(std::vector<uint8_t>& data) { return 0 ; };
    };


    template<>
    struct message<MsgIntro> : public message_base 
    {
        size_t deserialize(uint8_t* data, size_t size) {
            size_t res = message_base::deserialize(data, size);
            if(size - res >= 10) {
                mirror = *reinterpret_cast<uint32_t*>(data+res);
                port = *reinterpret_cast<uint16_t*>(data+res+4);
                version = *reinterpret_cast<uint16_t*>(data+res+6);
                res += 10;
            }
            return res;
        }
        size_t serialize(std::vector<uint8_t>& data) {
            data.insert(data.end(), (uint8_t*)&mirror, ((uint8_t*)&mirror)+4);
            data.insert(data.end(), (uint8_t*)&port, ((uint8_t*)&port)+2);
            data.insert(data.end(), (uint8_t*)&version, ((uint8_t*)&version)+4);
            return 10;
        }
        int32_t mirror;
        int32_t version;
        int16_t port;
    };

    template<>
    struct message<MsgGetBlocks>  : public message_base 
    {
        uint64_t last_block;
        uint64_t requested_blocks;
    };

    template<>
    struct message<MsgGiveBlocks> : public message_base
    {
        size_t deserialize(uint8_t* data, size_t len) {
            size_t res = message_base::deserialize(data, len);
            if( res > 0 ) {
                res += get_blocks(data+res, len-res, blocks);
            }

            return res;
        }
        size_t serialize(std::vector<uint8_t>& data) {

            std::vector<uint8_t> tmp_data;
            size_t res = set_blocks(blocks, tmp_data);
            size = res+4; // don't count size field, just name.
            res += message_base::serialize(data);
            data.insert(data.end(), tmp_data.begin(), tmp_data.end());

            return res;
        }
        std::vector<block> blocks;
    };

    template<>
    struct message<MsgGivePeers> : public message_base 
    {
        size_t deserialize(uint8_t* data, size_t len) {
            size_t res = message_base::deserialize(data, len);
            if(res > 0) {
                res += get_peers(data + res, len - res, peers);
            }
            return res;
        }
        size_t serialize(std::vector<uint8_t>& data) {
            std::vector<uint8_t> tmp_data;
            size_t res = set_peers(peers, tmp_data);
            size = res+4;
            res += message_base::serialize(data);
            data.insert(data.end(), tmp_data.begin(), tmp_data.end());

            return res;
        }

        std::vector<peer> peers;
    };

    //
    //  Message factory.  Usage
    //  auto msg = message_factory("INTR");
    //
    //  Alternatively you can specify a size that is the amount of space you want to 
    //  allocate for the message. Useful for messages that dont have a static size.
    //
#define CASE_4CC(x) case fourcc(x): \
                        res = std::make_unique<message<fourcc(x)>>(); \
                        res->name = x; \
                        res->size = size; \
                    break;

    inline std::unique_ptr<message_base> message_factory(fourcc_t name, size_t size = 0) {
        std::unique_ptr<message_base> res;
        switch(name()) {
            CASE_4CC("INTR");
            CASE_4CC("GIVB");
            CASE_4CC("GETB");
            CASE_4CC("GIVP");
        }
        return res;
    }

    template<int32_t Name>
    inline message<Name>* message_cast(message_base* msg) {
        if(msg->name.name == Name) {
            return static_cast<message<Name>*>(msg);
        } else {
            return nullptr;
        }
    }
} // namespace 
}

#endif