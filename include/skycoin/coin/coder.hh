#ifndef SKYCOIN__COIN_CODER
#define SKYCOIN__COIN_CODER

#include <experimental/optional>
#include <string>
#include <vector>

#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>
// This codec seems to be based on golang's encoding/binary

namespace skycoin { namespace coin {

    namespace stx = std::experimental;
    namespace detail {

        // Integers appear to be little-endian in this protocol.
        // Almost nobody uses big-endian these days so I will defer BE checking until
        // someone makes an issue complaining about it.
        
        template<typename T>
        struct get {
            stx::optional<T> operator()(uint8_t** p, const size_t remain) { 
                stx::optional<T> res ; 
                if(sizeof(T) <= remain) { res.emplace(*(T*)*p); *p += sizeof(T); } 
                return res; 
            }
        };

        template<>
        struct get<std::string> {
            stx::optional<std::string> operator()(uint8_t** p, const size_t remain) { 
                auto len = get<uint32_t>()(p, remain);
                stx::optional<std::string> res;
                if(len && *len <= (remain - 4)) {
                    res.emplace(reinterpret_cast<char*>(*p), *len);
                    *p += *len;
                }
                return res;
            }
        };

        template<>
        struct get<std::vector<uint8_t>> {
            stx::optional<std::vector<uint8_t>> operator()(uint8_t** p, const size_t remain) {
                auto len = get<uint32_t>()(p, remain);
                stx::optional<std::vector<uint8_t>> res;
                if(len && *len <= (remain - 4)) {
                    res.emplace(*p, *p+*len);
                    *p += *len;
                }
                return res;
            }
        };

        template<typename T, size_t S>
        struct get<std::array<T,S>> {
            stx::optional<std::array<T, S>> operator()(uint8_t** p, const size_t remain) {
                stx::optional<std::array<T, S>> res;
                uint32_t bytes = S * sizeof(T);
                if(bytes <= remain) {
                    res.emplace();
                    memcpy(res->data(), *p, bytes);
                    *p += bytes;
                }
                return res;
            }
        };

        template<typename T>
        struct get<std::vector<T>> {
            stx::optional<std::vector<T>> operator()(uint8_t** p, const size_t remain) {
                stx::optional<std::vector<T>> res;
                uint32_t count = 0;
                size_t size = remain;
                auto ct = get<uint32_t>()(p, size);
                if(ct) {
                    count = *ct;
                    size -= 4;
                    res.emplace();
                    for(uint32_t i = 0; i < count; i++) {
                        auto val = get<T>()(p, size);
                        if(val) {
                            size -= sizeof(T);
                            res->push_back(std::move(*val));
                        } else {
                            // error;
                            break;
                        }
                    }
                }
                return res;
            }
        };
        
        template<typename T>
        struct set {
            size_t operator()(T& val, std::vector<uint8_t>& buffer) {
                buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&val), reinterpret_cast<uint8_t*>(&val)+sizeof(T));
                return sizeof(T);
            }
        };

        template<>
        struct set<std::string> {
            size_t operator()(std::string& val, std::vector<uint8_t>& buffer) {
                uint32_t len(val.size());
                size_t res = set<uint32_t>()(len, buffer);
                buffer.insert(buffer.end(), val.begin(), val.end());
                return res + len;
            }
        };

        template<typename T, size_t S>
        struct set<std::array<T, S>> {
            size_t operator()(std::array<T, S>& val, std::vector<uint8_t>& buffer) {
                buffer.insert(buffer.end(), reinterpret_cast<uint8_t*>(&val[0]), reinterpret_cast<uint8_t*>(&val[0])+(S*sizeof(T)));
                return S*sizeof(T);
            }
        };

        template<typename T>
        struct set<std::vector<T>> {
            size_t operator()(std::vector<T>& val, std::vector<uint8_t>& buffer) {
                uint32_t count(val.size());
                size_t res = set<uint32_t>()(count, buffer);
                for(auto& it : val) {
                    res += set<T>()(it, buffer);
                }
                return res;
            }
        };
    }

    class decoder {
    public:

        decoder(uint8_t* data, size_t size)
        : base_(data), p_(data), size_(size) {};

        
        template<typename T>
        stx::optional<T> get() { return detail::get<T>()(&p_, size_ - (p_ - base_)); }

        template<typename T>
        bool safe_get(T& val) {
            auto res = get<T>();
            if(res) {
                val = *res;
            }
            return !!res;
        }
        
        bool done() const { return (p_ - base_) >= static_cast<intptr_t>(size_); }
        size_t remain() const { return size_ - (p_ - base_); };
        uint8_t* p() const { return p_; };
        void advance(size_t size) { p_ += size; };
    private:
        uint8_t* base_;
        uint8_t* p_;
        size_t   size_;
    };

    class encoder {
    public:
        encoder(std::vector<uint8_t>& data) : data_(data) {};

        template<typename T>
        size_t set(T& val) { return detail::set<T>()(val, data_); }

        const uint8_t* data() const { return data_.data(); }
        const size_t size() const { return data_.size(); }

    private:
        std::vector<uint8_t>& data_;
    };
}
}

#endif // SKYCOIN__NET_CODER