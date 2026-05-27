#pragma once
#include "web/common/def.h"
#include "web/common/message/msgdef.h"
#include <optional>
#include "serialization.h"
#include "web/server/message/data_reply_index_info.h"

namespace network{
template<>
    struct Message<network::Server_MsgT::DATA_REPLY_INDEX> final:public Message<Message_t<Side::SERVER>::DATA_REPLY_INDEX_REF>
    {
        std::string filename;
        uintmax_t file_sz_ = 0;      //size of file
        public:
        Message(const Message& other):
            Message<Message_t<Side::SERVER>::DATA_REPLY_INDEX_REF>(other),
            filename(other.filename),
            file_sz_(other.file_sz_){}
        Message(Message&& other) noexcept:
            Message<Message_t<Side::SERVER>::DATA_REPLY_INDEX_REF>(std::move(other)),
            filename(std::move(other.filename)),
            file_sz_(std::move(other.file_sz_)){}
        Message& operator=(const Message& other){
            if(this!=&other){
                Message<Message_t<Side::SERVER>::DATA_REPLY_INDEX_REF>::operator=(other);
                filename=other.filename;
                file_sz_=other.file_sz_;
            }
            return *this;
        }
        Message& operator=(Message&& other) noexcept{
            if(this!=&other){
                Message<Message_t<Side::SERVER>::DATA_REPLY_INDEX_REF>::operator=(std::move(other));
                filename=std::move(other.filename);
                file_sz_=std::move(other.file_sz_);
            }
            return *this;
        }
        Message() = default;
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Server_MsgT::DATA_REPLY_INDEX>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_INDEX>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,dynamic_cast<const network::Message<network::Server_MsgT::DATA_REPLY_INDEX_REF>&>(msg),
                    msg.filename,msg.file_sz_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Server_MsgT::DATA_REPLY_INDEX>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_INDEX>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,dynamic_cast<network::Message<network::Server_MsgT::DATA_REPLY_INDEX_REF>&>(msg),
                    msg.filename,msg.file_sz_);
        }
    };

    template<>
    struct Serial_size<network::Message<network::Server_MsgT::DATA_REPLY_INDEX>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_INDEX>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(dynamic_cast<const network::Message<network::Server_MsgT::DATA_REPLY_INDEX_REF>&>(msg),
                    msg.filename,msg.file_sz_);
        }
    };

    template<>
    struct Min_serial_size<network::Message<network::Server_MsgT::DATA_REPLY_INDEX>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_INDEX>;
        static constexpr size_t value = []()
        {
            return min_serial_size<network::Message<network::Server_MsgT::DATA_REPLY_INDEX_REF>,decltype(type::filename),decltype(type::file_sz_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::Message<network::Server_MsgT::DATA_REPLY_INDEX>>{
        using type = network::Message<network::Server_MsgT::DATA_REPLY_INDEX>;
        static constexpr size_t value = []()
        {
            return max_serial_size<network::Message<network::Server_MsgT::DATA_REPLY_INDEX_REF>,decltype(type::filename),decltype(type::file_sz_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Server_MsgT::DATA_REPLY_INDEX>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Server_MsgT::DATA_REPLY_INDEX>>);
static_assert(serialization::deserialize_concept<true,network::BaseIndexResult<Data_t::TIME_SERIES, Data_f::GRIB_v1>>);
static_assert(serialization::deserialize_concept<false,network::BaseIndexResult<Data_t::TIME_SERIES, Data_f::GRIB_v1>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Server_MsgT::DATA_REPLY_INDEX>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Server_MsgT::DATA_REPLY_INDEX>>);
static_assert(serialization::serialize_concept<true,network::BaseIndexResult<Data_t::TIME_SERIES, Data_f::GRIB_v1>>);
static_assert(serialization::serialize_concept<false,network::BaseIndexResult<Data_t::TIME_SERIES, Data_f::GRIB_v1>>);