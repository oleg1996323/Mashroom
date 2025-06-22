#pragma once
#include <span>
#include <vector>
#include <netdb.h>
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "network/common/def.h"
namespace network{
    ErrorCode receive(Socket socket,std::vector<char>& buffer, size_t n) noexcept{
        ssize_t read = recv(socket,buffer.data(),n,MSG_WAITALL);
    }
}