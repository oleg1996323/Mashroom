#pragma once
#include <thread>
class AbstractThreadInterruptor{
    protected:
    std::stop_token stop_token_;
    public:
    void set_stop_token(std::stop_token token){
        stop_token_ = token;
    }
    std::stop_token token() const{
        return stop_token_;
    }
    ~AbstractThreadInterruptor() = default;
};