#include "web/common/detail/hash.h"

namespace network{
    namespace detail{
        std::string to_string(const boost::uuids::detail::md5::digest_type &digest) noexcept{
            using boost::uuids::detail::md5;
            const auto intDigest = reinterpret_cast<const int*>(&digest);
            std::string result;
            boost::algorithm::hex(intDigest, intDigest + (sizeof(md5::digest_type)/sizeof(int)), std::back_inserter(result));
            return result;
        }
        std::string to_string(const boost::uuids::detail::sha1::digest_type &digest) noexcept{
            using boost::uuids::detail::sha1;
            const auto intDigest = reinterpret_cast<const int*>(&digest);
            std::string result;
            boost::algorithm::hex(intDigest, intDigest + (sizeof(sha1::digest_type)/sizeof(int)), std::back_inserter(result));
            return result;
        }
    }
}