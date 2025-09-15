#pragma once
#include <string>
#include <cstdint>
#include "definitions/protocol.h"

namespace network::server{
struct Settings{
    std::string host;
    std::string service;
    Protocol protocol = Protocol::TCP;
    int timeout_seconds_=20;
    int32_t port;
    Settings() = default;
    Settings(const Settings& other)
    {
        if(this!=&other){
            host=other.host;
            service=other.service;
            port=other.port;
            protocol=other.protocol;
            timeout_seconds_=other.timeout_seconds_;
        }
    }
    Settings(Settings&& other)
    {   
        if(this!=&other){
            host=std::move(other.host);
            service=std::move(other.service);
            port=std::move(other.port);
            protocol=std::move(other.protocol);
            timeout_seconds_=other.timeout_seconds_;
        }
    }
    Settings& operator=(const Settings& other){
        if(this!=&other){
            host=other.host;
            service=other.service;
            port=other.port;
            protocol=other.protocol;
            timeout_seconds_=other.timeout_seconds_;
        }
        return *this;
    }
    Settings& operator=(Settings&& other){
        if(this!=&other){
            host.swap(other.host);
            service.swap(other.service);
            port = other.port;
            protocol = other.protocol;
            timeout_seconds_=other.timeout_seconds_;
        }
        return *this;
    }  
};
}