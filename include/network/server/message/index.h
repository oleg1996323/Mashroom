#pragma once
#include <network/common/def.h>
#include <network/common/message/msgdef.h>
#include <optional>
#include "serialization.h"
#include "network/server/message/data_reply_index_info.h"

namespace network{
template<>
    struct MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX> final:public MessageAdditional<Message_t<Side::SERVER>::DATA_REPLY_INDEX_REF>
    {
        std::string filename;
        uintmax_t file_sz_ = 0;      //size of file
        public:
        MessageAdditional(const MessageAdditional& other):
            MessageAdditional<Message_t<Side::SERVER>::DATA_REPLY_INDEX_REF>(other),
            filename(other.filename),
            file_sz_(other.file_sz_){}
        MessageAdditional(MessageAdditional&& other) noexcept:
            MessageAdditional<Message_t<Side::SERVER>::DATA_REPLY_INDEX_REF>(std::move(other)),
            filename(std::move(other.filename)),
            file_sz_(std::move(other.file_sz_)){}
        MessageAdditional& operator=(const MessageAdditional& other){
            if(this!=&other){
                MessageAdditional<Message_t<Side::SERVER>::DATA_REPLY_INDEX_REF>::operator=(other);
                filename=other.filename;
                file_sz_=other.file_sz_;
            }
            return *this;
        }
        MessageAdditional& operator=(MessageAdditional&& other) noexcept{
            if(this!=&other){
                MessageAdditional<Message_t<Side::SERVER>::DATA_REPLY_INDEX_REF>::operator=(std::move(other));
                filename=std::move(other.filename);
                file_sz_=std::move(other.file_sz_);
            }
            return *this;
        }
        MessageAdditional() = default;
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,dynamic_cast<const network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>&>(msg),
                    msg.filename,msg.file_sz_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,dynamic_cast<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>&>(msg),
                    msg.filename,msg.file_sz_);
        }
    };

    template<>
    struct Serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(dynamic_cast<const network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>&>(msg),
                    msg.filename,msg.file_sz_);
        }
    };

    template<>
    struct Min_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>;
        static constexpr size_t value = []()
        {
            return min_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>,decltype(type::filename),decltype(type::file_sz_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>;
        static constexpr size_t value = []()
        {
            return max_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>,decltype(type::filename),decltype(type::file_sz_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>>);
static_assert(serialization::deserialize_concept<false,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>>);
static_assert(serialization::deserialize_concept<true,network::BaseIndexResult<Data_t::METEO, Data_f::GRIB>>);
static_assert(serialization::deserialize_concept<false,network::BaseIndexResult<Data_t::METEO, Data_f::GRIB>>);
static_assert(serialization::serialize_concept<true,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>>);
static_assert(serialization::serialize_concept<false,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX>>);
static_assert(serialization::serialize_concept<true,network::BaseIndexResult<Data_t::METEO, Data_f::GRIB>>);
static_assert(serialization::serialize_concept<false,network::BaseIndexResult<Data_t::METEO, Data_f::GRIB>>);