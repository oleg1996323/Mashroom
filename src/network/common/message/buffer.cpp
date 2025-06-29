#include "network/common/message/buffer.h"
#include "network/common/message/cashed_data.h"
#include "network/common/message/msgdef.h"

namespace network{
    void __MessageBuffer__::assign_buffer(std::vector<char>&& buf) noexcept{
        if(&buf!=&__buffer__())
            __buffer__().swap(buf);
    }
    void __MessageBuffer__::assign_buffer(const std::vector<char>& buf) const{
        if(&buf!=&__buffer__())
            __buffer__() = buf;
    }
    //add assignment buffer
    std::vector<char>& __MessageBuffer__::__buffer__() const{
        return CashedData::get(this);
    }
    __MessageBuffer__::~__MessageBuffer__(){
        CashedData::erase(this);
    }
}