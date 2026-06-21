#pragma once
#include <string>
#include <boost/uuid/detail/md5.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/algorithm/hex.hpp>
#include "serialization.h"

namespace network{
    
    enum HashAlgorithm{
        NONE,
        MD5,
        SHA1
    };

    namespace detail{
        std::string to_string(const boost::uuids::detail::md5::digest_type &digest) noexcept;
        std::string to_string(const boost::uuids::detail::sha1::digest_type &digest) noexcept;
    }

    template<typename DIGEST>
    std::string digest_to_string(const DIGEST& digest) noexcept{
        using boost::uuids::detail::md5;
        using boost::uuids::detail::sha1;
        static_assert(std::is_same_v<sha1,DIGEST> ||
                    std::is_same_v<md5,DIGEST>);
        return detail::to_string(digest);
    }
    
    class Hash{
        template<bool,auto>
        friend struct serialization::Serialize;
        template<bool,auto>
        friend struct serialization::Deserialize;
        template<auto>
        friend struct serialization::Serial_size;
        template<auto>
        friend struct serialization::Min_serial_size;
        template<auto>
        friend struct serialization::Max_serial_size;
        std::string hash_;
        HashAlgorithm algo_;
        public:
        Hash(const boost::uuids::detail::md5::digest_type& digest):
            hash_(std::move(detail::to_string(digest))),
            algo_(HashAlgorithm::MD5){}
        Hash(const boost::uuids::detail::sha1::digest_type& digest):
            hash_(std::move(detail::to_string(digest))),
            algo_(HashAlgorithm::SHA1){}
        const std::string& value() const noexcept{
            return hash_;
        }
        HashAlgorithm algorithm() const noexcept{
            return algo_;
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Hash>{
        using type = Hash;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.hash_,msg.algo_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Hash>{
        using type = Hash;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.hash_,msg.algo_);
        }
    };

    template<>
    struct Serial_size<Hash>{
        using type = Hash;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.hash_,msg.algo_);
        }
    };

    template<>
    struct Min_serial_size<Hash>{
        using type = Hash;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::hash_),
                        decltype(type::algo_)>();
        }();
    };

    template<>
    struct Max_serial_size<Hash>{
        using type = Hash;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::hash_),
                        decltype(type::algo_)>();
        }();
    };
}