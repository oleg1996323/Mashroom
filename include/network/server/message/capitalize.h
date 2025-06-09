#pragma once
#include "msgdef.h"
#include <optional>
#include <network/common/def.h>
#include <network/common/message_handler.h>
#include <extract.h>
#include <program/data.h>
using namespace std::chrono;
namespace fs = std::filesystem;
namespace network::server{

template<>
class Message<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE>:public __Message__<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE>{
    struct BaseCapitalizeResult{
        const Data::TYPE data_type;
        const Data::FORMAT data_format;
        std::vector<char> buffer_;
    };
    template<Data::TYPE T,Data::FORMAT F>
    struct __CAPITALIZE_RESULT__:BaseCapitalizeResult{
        __CAPITALIZE_RESULT__():BaseCapitalizeResult{T,F}{}
        

        static std::vector<char> serialize(const __CAPITALIZE_RESULT__& msg){
            return __CAPITALIZE_RESULT__::serialize(msg);
        }
        static __CAPITALIZE_RESULT__ deserialize(std::vector<char>::const_iterator buffer_iter){
            return __CAPITALIZE_RESULT__::deserialize(buffer_iter);
        }
    };
    uint64_t blocks_ = 0;

    Message(    ErrorCode err,
                server::Status status = server::Status::READY):
    __Message__<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE>(status,0){
        
    }
    private:
    ErrorCode serialize_impl() const;
    ErrorCode deserialize_impl();
    public:
    using associated_t = BaseCapitalizeResult;
    bool add_block(Data::TYPE T,Data::FORMAT F, Data::ACCESS A);
};
}