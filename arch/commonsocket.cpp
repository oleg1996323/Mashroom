#include "commonsocket.h"
#include "OsterLib/byte_order.h"

namespace network{
    detail::SocketControlBlock::SocketControlBlock(int raw_socket_id,
        const sockaddr_storage& stor):
        socket_(raw_socket_id),
        storage_(stor){}
}

network::detail::Socket::Options operator|(
        network::detail::Socket::Options lhs,
        network::detail::Socket::Options rhs) noexcept{
    return static_cast<network::detail::Socket::Options>(
        static_cast<std::underlying_type_t<network::detail::Socket::Options>>(lhs)|
        static_cast<std::underlying_type_t<network::detail::Socket::Options>>(rhs));
}
network::detail::Socket::Options operator&(
        network::detail::Socket::Options lhs,
        network::detail::Socket::Options rhs) noexcept{
    return static_cast<network::detail::Socket::Options>(
        static_cast<std::underlying_type_t<network::detail::Socket::Options>>(lhs)&
        static_cast<std::underlying_type_t<network::detail::Socket::Options>>(rhs));
}
network::detail::Socket::Options operator^(
        network::detail::Socket::Options lhs,
        network::detail::Socket::Options rhs) noexcept{
    return static_cast<network::detail::Socket::Options>(
        static_cast<std::underlying_type_t<network::detail::Socket::Options>>(lhs)^
        static_cast<std::underlying_type_t<network::detail::Socket::Options>>(rhs));
}
network::detail::Socket::Options operator~(
        network::detail::Socket::Options val) noexcept{
    return static_cast<network::detail::Socket::Options>(
        ~static_cast<std::underlying_type_t<network::detail::Socket::Options>>(val));
}
network::detail::Socket::Options operator&(
        network::detail::Socket::Options lhs,int rhs) noexcept{
    return static_cast<network::detail::Socket::Options>(
            static_cast<std::underlying_type_t<
            network::detail::Socket::Options>>(lhs)&rhs);
}
network::detail::Socket::Options operator&(
        std::underlying_type_t<network::detail::Socket::Options> lhs,
        network::detail::Socket::Options rhs) noexcept{
    return static_cast<network::detail::Socket::Options>(lhs &
            static_cast<std::underlying_type_t<
            network::detail::Socket::Options>>(rhs));
}
network::detail::Socket::Options operator|(
        network::detail::Socket::Options lhs,
        std::underlying_type_t<network::detail::Socket::Options> rhs) noexcept{
    return static_cast<network::detail::Socket::Options>(
        static_cast<std::underlying_type_t<
        network::detail::Socket::Options>>(lhs)|rhs);
}
network::detail::Socket::Options operator|(
        std::underlying_type_t<network::detail::Socket::Options> lhs,
        network::detail::Socket::Options rhs) noexcept{
    return static_cast<network::detail::Socket::Options>(lhs |
        static_cast<std::underlying_type_t<
        network::detail::Socket::Options>>(rhs));
}

network::detail::Socket::Type operator|(
        network::detail::Socket::Type lhs,
        network::detail::Socket::Type rhs) noexcept{
    return static_cast<network::detail::Socket::Type>(
        static_cast<std::underlying_type_t<
            network::detail::Socket::Type>>(lhs)|
        static_cast<std::underlying_type_t<
            network::detail::Socket::Type>>(rhs));
}
network::detail::Socket::Type operator&(
        network::detail::Socket::Type lhs,
        network::detail::Socket::Type rhs) noexcept{
    return static_cast<network::detail::Socket::Type>(
        static_cast<std::underlying_type_t<
            network::detail::Socket::Type>>(lhs)&
        static_cast<std::underlying_type_t<
            network::detail::Socket::Type>>(rhs));
}
network::detail::Socket::Type operator^(
        network::detail::Socket::Type lhs,
        network::detail::Socket::Type rhs) noexcept{
    return static_cast<network::detail::Socket::Type>(
        static_cast<std::underlying_type_t<
            network::detail::Socket::Type>>(lhs)^
        static_cast<std::underlying_type_t<
            network::detail::Socket::Type>>(rhs));
}
network::detail::Socket::Type operator~(
        network::detail::Socket::Type val) noexcept{
    return static_cast<network::detail::Socket::Type>(
        ~static_cast<std::underlying_type_t<
            network::detail::Socket::Type>>(val));
}
network::detail::Socket::Type operator&(
        network::detail::Socket::Type lhs,int rhs) noexcept{
    return static_cast<network::detail::Socket::Type>(
            static_cast<std::underlying_type_t<
            network::detail::Socket::Type>>(lhs)&rhs);
}
network::detail::Socket::Type operator&(
        std::underlying_type_t<network::detail::Socket::Type> lhs,
        network::detail::Socket::Type rhs) noexcept{
    return  static_cast<network::detail::Socket::Type>(lhs &
            static_cast<std::underlying_type_t<
                network::detail::Socket::Type>>(rhs));
}
network::detail::Socket::Type operator|(
        network::detail::Socket::Type lhs,
        std::underlying_type_t<network::detail::Socket::Type> rhs) noexcept{
    return  static_cast<network::detail::Socket::Type>(
            static_cast<std::underlying_type_t<
                network::detail::Socket::Type>>(lhs)|rhs);
}
network::detail::Socket::Type operator|(
        std::underlying_type_t<network::detail::Socket::Type> lhs,
        network::detail::Socket::Type rhs) noexcept{
    return static_cast<network::detail::Socket::Type>(lhs |
            static_cast<std::underlying_type_t<
                network::detail::Socket::Type>>(rhs));
}