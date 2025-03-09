#pragma once
#include <inttypes.h>
#include <stdlib.h>

class Size
{
private:
    uint32_t w_;
    uint32_t h_;
public:
    Size(uint32_t w, uint32_t h);
    ~Size() = default;

    inline uint32_t height() const{
        return h_;
    }
    inline uint32_t width() const{
        return w_;
    }
    inline uint32_t& rheight(){
        return h_;
    }
    inline uint32_t& rwidth(){
        return w_;
    }
};