#pragma once
#include <string>
#include <cstdint>
#include "definitions/protocol.h"

namespace network::server{
struct Settings{
    std::string host_;
    std::string service_;
    Protocol protocol_ = Protocol::TCP;
    int timeout_seconds_=20;
    int32_t port_;
    bool reuse_address_ = true;
    Settings() = default;
    Settings(std::string host,std::string service,Protocol proto,int timeout,int32_t port,bool reuse_address):
            host_(host),
            service_(service),
            protocol_(proto),
            timeout_seconds_(timeout),
            port_(port),
            reuse_address_(reuse_address){}
    Settings(const Settings& other)
    {
        if(this!=&other){
            host_=other.host_;
            service_=other.service_;
            port_=other.port_;
            protocol_=other.protocol_;
            timeout_seconds_=other.timeout_seconds_;
            reuse_address_ = other.reuse_address_;
        }
    }
    Settings(Settings&& other)
    {   
        if(this!=&other){
            host_=std::move(other.host_);
            service_=std::move(other.service_);
            port_=std::move(other.port_);
            protocol_=std::move(other.protocol_);
            timeout_seconds_=other.timeout_seconds_;
            reuse_address_ = other.reuse_address_;
        }
    }
    Settings& operator=(const Settings& other){
        if(this!=&other){
            host_=other.host_;
            service_=other.service_;
            port_=other.port_;
            protocol_=other.protocol_;
            timeout_seconds_=other.timeout_seconds_;
            reuse_address_ = other.reuse_address_;
        }
        return *this;
    }
    Settings& operator=(Settings&& other){
        if(this!=&other){
            host_.swap(other.host_);
            service_.swap(other.service_);
            port_ = other.port_;
            protocol_ = other.protocol_;
            timeout_seconds_=other.timeout_seconds_;
            reuse_address_ = other.reuse_address_;
        }
        return *this;
    }  
};
}