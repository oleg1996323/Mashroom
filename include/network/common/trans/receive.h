#pragma once
#include <span>
#include <vector>
#include <ranges>
#include <netdb.h>
#include "network/common/def.h"
namespace network{
    /**
     * @brief If there is an error, -1 is returned.
     */
    int receive(const Socket& socket,std::ranges::random_access_range auto& buffer, uint64_t n) noexcept{
        if(n==0)
            return 0;
        assert(buffer.size()>=n);
        ssize_t read_num = 0;
        while(read_num!=n){
            read_num = recv(socket.__descriptor__(),buffer.data(),n,MSG_WAITALL);
            if(read_num<0){
                if(read_num==EINTR)
                    read_num=0;
                else
                    return -1;
            }
            else continue;
        }
        return 0;
    }
}