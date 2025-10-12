#pragma once
#include <network/common/message/msgdef.h>
#include "progress_base.h"
#include <variant>
#include "data/datastruct.h"

namespace network{

template<Data_t T, Data_f F>
struct BaseIndexResult{
    using data_t = DataStruct<T,F>::find_all_t;
    data_t data_;
};

using IndexResult = std::variant<std::monostate,
        BaseIndexResult<Data_t::METEO, Data_f::GRIB>>;

template<>
struct MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>
{
    std::vector<IndexResult> blocks_;
    server::Status status_;

    public:
    MessageAdditional(const MessageAdditional& other):blocks_(other.blocks_){}
    MessageAdditional(MessageAdditional&& other) noexcept:blocks_(std::move(other.blocks_)){}
    MessageAdditional& operator=(const MessageAdditional& other){
        if(this!=&other)
            blocks_ = other.blocks_;
        return *this;
    }
    MessageAdditional& operator=(MessageAdditional&& other) noexcept{
        if(this!=&other)
            blocks_ = std::move(other.blocks_);
        return *this;
    }
    MessageAdditional() = default;
    template<Data_t T,Data_f F,typename Type>
    bool add_block(Data_a A, Type&& block){
        BaseIndexResult<T,F> result;
        result.data_ = std::move(block);
        blocks_.emplace_back(std::move(result));
        return true;
    }
};
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.blocks_,msg.status_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.blocks_,msg.status_);
        }
    };

    template<>
    struct Serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.blocks_,msg.status_);
        }
    };

    template<>
    struct Min_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::blocks_),decltype(type::status_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::blocks_),decltype(type::status_)>();
        }();
    };

    template<bool NETWORK_ORDER,Data_t T, Data_f F>
    struct Serialize<NETWORK_ORDER,network::BaseIndexResult<T, F>>{
        using type = network::BaseIndexResult<T, F>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.data_);
        }
    };

    template<bool NETWORK_ORDER,Data_t T, Data_f F>
    struct Deserialize<NETWORK_ORDER,network::BaseIndexResult<T, F>>{
        using type = network::BaseIndexResult<T, F>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.data_);
        }
    };

    template<Data_t T, Data_f F>
    struct Serial_size<network::BaseIndexResult<T, F>>{
        using type = network::BaseIndexResult<T, F>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.data_);
        }
    };

    template<Data_t T, Data_f F>
    struct Min_serial_size<network::BaseIndexResult<T, F>>{
        using type = network::BaseIndexResult<T, F>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::data_)>();
        }();
    };

    template<Data_t T, Data_f F>
    struct Max_serial_size<network::BaseIndexResult<T, F>>{
        using type = network::BaseIndexResult<T, F>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::data_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>>);
static_assert(serialization::deserialize_concept<false,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>>);
static_assert(serialization::serialize_concept<true,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>>);
static_assert(serialization::serialize_concept<false,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_INDEX_REF>>);
