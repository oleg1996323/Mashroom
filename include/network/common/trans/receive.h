#pragma once
#include <span>
#include <vector>
#include <ranges>
#include <netdb.h>
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "network/common/def.h"
namespace network{
    ErrorCode receive(Socket socket,std::ranges::random_access_range auto& buffer, uint64_t n) noexcept{
        if(n==0)
            return ErrorCode::NONE;
        ssize_t read_num = recv(socket,buffer.data(),n,MSG_WAITALL);
        if(read_num<=0)
            return ErrorCode::RECEIVING_MESSAGE_ERROR;
        return ErrorCode::NONE;
    }
}