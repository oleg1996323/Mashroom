#pragma once
#include "definitions/family.h"
#include "definitions/protocol.h"
#include <cstdint>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

namespace network{
    using Port = uint16_t;
    using FileDescriptor = int;

    std::ostream& print_ip_port(std::ostream& stream,const sockaddr_storage& addr);
    std::string ip_to_text(const sockaddr_storage& addr);
    std::string port_to_text(const sockaddr_storage& addr);
    std::string protocol_to_text(const sockaddr_storage& addr);
    bool is_correct_address(const std::string& text) noexcept;
    bool is_correct_address(std::string_view text) noexcept;
    socklen_t address_struct_size(const sockaddr_storage& storage) noexcept;
}