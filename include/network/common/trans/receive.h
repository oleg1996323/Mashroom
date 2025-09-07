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
        if(buffer.size()<n)
            return ErrorPrint::print_error(ErrorCode::BUFFER_LOW_SIZE,"at receiving message",AT_ERROR_ACTION::CONTINUE);
        ssize_t read_num = 0;
        while(read_num!=n){
            read_num = recv(socket,buffer.data(),n,MSG_WAITALL);
            if(read_num<0){
                if(read_num==EINTR)
                    read_num=0;
                else
                    return ErrorPrint::print_error(ErrorCode::RECEIVING_MESSAGE_ERROR,strerror(read_num),AT_ERROR_ACTION::CONTINUE);
            }
            else continue;
        }
        return ErrorCode::NONE;
    }
}