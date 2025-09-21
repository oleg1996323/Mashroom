#pragma once
#include <span>
#include <vector>
#include <netdb.h>
#include "definitions.h"
#include <cstring>
#include <ranges>
#include <cstdint>
#include "commonsocket.h"
#include <iostream>

namespace network{
    template<std::ranges::random_access_range... ARGS>
    int send(const Socket& socket,const std::ranges::random_access_range auto&... buffers) noexcept{
        auto send_sequentialy = [&](auto&& buffer,int flags) noexcept -> bool
        {
            size_t sent = 0;
            if(buffer.size()==0)
                return true;
            while(sent<buffer.size()){
                ssize_t cur = ::send(socket.__descriptor__(),buffer.data()+sent,buffer.size()-sent,flags|MSG_NOSIGNAL);
                if(cur==-1)
                    return false;
                else sent+=cur;
                if(sent<buffer.size())
                    std::cout<<"Sending resting message"<<std::endl;
            }
            return true;
        };

        size_t i = 0;
        constexpr size_t count = sizeof...(buffers);
        auto send_with_flags = [&](auto&& buf) noexcept{
            return send_sequentialy(buf, (++i < count) ? 0/* (MSG_MORE | MSG_DONTWAIT) */ : 0);
        };

        if(!(send_with_flags(buffers) && ...)) {
            return -1;
        }
        return 0;
    }
}