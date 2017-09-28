#ifndef SKYCOIN__COIN_CODER
#define SKYCOIN__COIN_CODER

#include <experimental/optional>
#include <string>
#include <vector>

#include <arpa/inet.h>
#include <stdint.h>

// This codec seems to be based on golang's encoding/binary

namespace skycoin { namespace coin {

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

        template<typename T>
        void set(T& val, bytes& buffer) {
            buffer.insert(buffer.end(), (uint8_t*)val, ((uint8_t*)val)+sizeof(T));
        }

        template<>
        void set(std::string& val, bytes& buffer) {
            uint32_t len(val.size());
            set(len, buffer);
            buffer.insert(buffer.end(), val.begin(), val.end());
        }

        template<>
        void set(bytes& val, bytes& buffer) {
            uint32_t len(val.size());
            set(len, buffer);
            buffer.insert(buffer.end(), val.begin(), val.end());
        }
    }

    class decoder {
    public:

        decoder(uint8_t* data, size_t size)
        : base_(data), p_(data), size_(size) {};

        
        template<typename T>
        stx::optional<T> get() { return detail::get<T>(&p_, size_ - (p_ - base_)); }

        // in the case of a 0-length array it will get the length from
        // the datastream.  If a length is specified the length will 
        // be assumed to not exist in the datastream.
        template<typename T>
        stx::optional<std::vector<T>> get_array(uint32_t len = 0) {
            stx::optional<std::vector<T>> res;
            uint32_t count = len;
            if(!count) {
                auto b = detail::get<uint32_t>(&p_, size_ - (p_ - base_));
                if(b) {
                    count = *b;
                }
            }
            uint32_t bytes = count * sizeof(T);
            if(bytes <= (size_ - (p_ - base_))) {
                res.emplace(count);
                memcpy(res->data(), p_, bytes);
            }
            return res;
        }

        bool done() const { return (p_ - base_) >= size_; }

    private:
        uint8_t* base_;
        uint8_t* p_;
        size_t   size_;
    };

    class encoder {
    public:
        encoder() {};

        template<typename T>
        void set(T& val) { detail::set(val, data_); };

        const uint8_t* data() const { return data_.data(); }
        const size_t size() const { return data_.size(); }

    private:
        bytes data_;
    };
}
}

#endif // SKYCOIN__NET_CODER