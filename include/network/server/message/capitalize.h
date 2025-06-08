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
    template<Data::TYPE T,Data::FORMAT F>
    struct __CAPITALIZE_RESULT__{
        static constexpr Data::TYPE data_type = T;
        static constexpr Data::FORMAT data_format = F;
        static std::vector<char> serialize(const CapitalizeResult<T,F>& msg){
            return CapitalizeResult<T,F>::serialize(msg);
        }
        static CapitalizeResult<T,F> deserialize(std::vector<char>::const_iterator buffer_iter){
            return CapitalizeResult<T,F>::deserialize(buffer_iter);
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
    using associated_t = __CAPITALIZE_RESULT__;
    //TODO: set structure for reply message
    //maybe add data-type (topo,meteo,kadasr etc)
    bool sendto(int sock);
    bool receivefrom(int sock);

};
}