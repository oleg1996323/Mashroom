#pragma once
#include <span>
#include <vector>
#include <ranges>
#include <netdb.h>
#include "definitions.h"
#include "commonsocket.h"
namespace network{
    /**
     * @brief If there is an error, -1 is returned.
     */
    int receive(const Socket& socket,std::ranges::random_access_range auto& buffer, uint64_t n) noexcept{
        if(n==0)
            return 0;
        assert(buffer.size()>=n);
        ssize_t read_num = 0;
        uint16_t called = 0;
        while(n!=0){
            read_num = recv(socket.__descriptor__(),buffer.data()+buffer.size()-n,n,0);
            if(read_num<0){
                auto err = errno;
                errno= 0;
                if(read_num==EINTR)
                    read_num=0;
                else if(read_num==EAGAIN)
                    read_num=0;
                else{
                    std::cout<<"Receiving error: "<<strerror(err)<<"Error code: "<<errno<<std::endl;
                    return -1;
                }
            }
            else {
                ++called;
                n-=read_num;
                continue;
            }
        }
        std::cout<<"Called "<<called<<std::endl;
        return 0;
    }

    int receive(const Socket& socket,std::ranges::view auto buffer, uint64_t n) noexcept 
    requires std::ranges::random_access_range<decltype(buffer)>
    {
        if(n==0)
            return 0;
        assert(buffer.size()>=n);
        ssize_t read_num = 0;
        while(n!=0){
            read_num = recv(socket.__descriptor__(),buffer.data()+buffer.size()-n,n,0);
            if(read_num<0){
                std::cout<<"Receiving error: "<<strerror(errno)<<std::endl;
                errno= 0;
                if(read_num==EINTR)
                    read_num=0;
                else if(read_num==EAGAIN)
                    read_num=0;
                else
                    return -1;
            }
            else {
                n-=read_num;
                continue;
            }
        }
        return 0;
    }
}