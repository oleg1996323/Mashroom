#pragma once
#include <span>
#include <vector>
#include <netdb.h>
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "network/common/def.h"
#include <cstring>
#include <ranges>
namespace network{
    template<std::ranges::random_access_range... ARGS>
    // requires (std::is_same_v<std::decay_t<ARGS>,std::span<const char>> && ...)
    ErrorCode send(Socket socket,const ARGS&... buffers) noexcept{
        ErrorCode err = ErrorCode::NONE;
        auto send_sequentialy = [&](std::span<const char> buffer,int flags) -> ErrorCode
        {
            size_t sent = 0;
            if(buffer.size()==0)
                return ErrorCode::NONE;
            while(sent<buffer.size()){
                int cur = ::send(socket,buffer.data()+sent,buffer.size()-sent,flags);
                if(cur<0){
                    err = ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,std::strerror(errno),AT_ERROR_ACTION::CONTINUE);
                    return err;
                }
                else sent+=cur;
            }
            return ErrorCode::NONE;
        };

        size_t i = 0;
        constexpr size_t count = sizeof...(buffers);
        auto send_with_flags = [&](auto&& buf) {
            int flags = (++i < count) ? (MSG_MORE | MSG_DONTWAIT) : 0;
            return send_sequentialy(buf, flags) == ErrorCode::NONE;
        };

        if(!(send_with_flags(buffers) && ...)) {
            return err;
        }
        return ErrorCode::NONE;
    }
}