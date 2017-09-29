#ifndef SKYCOIN__COIN_MESSAGE_HH
#define SKYCOIN__COIN_MESSAGE_HH

#include <string>
#include <memory>

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

    enum {
        MessageIntro = fourcc("INTR"),
        MessageGetBlocks = fourcc("GETB")
    };

    // Depending on how much access is happening with these structs, a future
    // TODO would be to move from packed structs to aligned structs that serialize/deserialze.
    // This is a lazy implementaion for now though.
#pragma pack(push, 2)
    struct message_base 
    {
        virtual void realizer() {};
        uint32_t size;       // Size does not appear to be inclusive of the size field,
        fourcc_t name;       // but is inclusive of the name.
    };
    
    template<int32_t Name>
    struct message : public message_base
    {};


    template<>
    struct message<MessageIntro> : public message_base 
    {
        int32_t mirror;
        int16_t port;
        int32_t version;
    };

    template<>
    struct message<MessageGetBlocks>  : public message_base 
    {
        uint64_t last_block;
        uint64_t requested_blocks;
    };
#pragma pack(pop)
    //
    //  Message factory.  Usage
    //  auto msg = message_factory("INTR");
    //
    //  Alternatively you can specify a size that is the amount of space you want to 
    //  allocate for the message. Useful for messages that dont have a static size.
    //
#define CASE_4CC(x) case fourcc(x): \
                        size = std::max(size + sizeof(message<fourcc(x)>), sizeof(message<fourcc(x)>)); \
                        res.reset((message<fourcc(x)>*) ::operator new(size)); \
                        new (res.get()) message<fourcc(x)>; \
                        res->size = size-4; \
                        res->name = x; \
                    break;

    

    inline std::unique_ptr<message_base> message_factory(fourcc_t name, size_t size = 0) {
        std::unique_ptr<message_base> res;
        switch(name()) {
            CASE_4CC("INTR");
        }
        return res;
    }

} // namespace 
}

#endif