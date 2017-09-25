#ifndef SKYCOIN__NET_CODER
#define SKYCOIN__NET_CODER

#include <experimental/optional>
#include <string>
#include <vector>

#include <arpa/inet.h>
#include <stdint.h>

namespace skycoin { namespace coder {

    namespace stx = std::experimental;

    using bytes = std::vector<uint8_t>;

    namespace detail {

        // Integers appear to be little-endian in this protocol.
        // Almost nobody uses big-endian these days so I will defer BE checking until
        // someone makes an issue complaining about it.
        template<typename T>
        stx::optional<T> get(uint8_t** p, const size_t remain) { 
            stx::optional<T> res ; 
            if(sizeof(T) <= remain) { res.emplace(*(T*)*p); *p += sizeof(T); } 
            return res; 
        }

        template<>
        stx::optional<std::string> get(uint8_t** p, const size_t remain) {
            auto len = get<uint32_t>(p, remain);
            stx::optional<std::string> res;
            if(len && *len <= (remain - 4)) {
                res.emplace((const char*)*p, len);
                *p += *len;
            }
            return res;
        }

        template<>
        stx::optional<bytes> get(uint8_t** p, const size_t remain) {
            auto len = get<uint32_t>(p, remain);
            stx::optional<bytes> res;
            if(len && *len <= (remain - 4)) {
                res.emplace(*p, *p+*len);
                *p += *len;
            }
        }
    }

    class decoder {
    public:

        decoder(uint8_t* data, size_t size)
        : base_(data), p_(data), size_(size) {};

        
        template<typename T>
        stx::optional<T> get() { return detail::get<T>(&p_, size_ - (p_ - base_)); }

        bool done() const { return (p_ - base_) >= size_; }

    private:
        uint8_t* base_;
        uint8_t* p_;
        size_t   size_;
    };

    class encoder {


    };
}
}

#endif // SKYCOIN__NET_CODER