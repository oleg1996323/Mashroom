#pragma once
#include "network/common/message/msgdef.h"
#include <variant>
#include <cstdint>
#include <cstddef>

template <Data::TYPE T, Data::FORMAT F>
struct ExtractRequestForm;

template <>
struct ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB>
{
    constexpr static Data::TYPE type = Data::TYPE::METEO;
    constexpr static Data::FORMAT format = Data::FORMAT::GRIB;
    std::optional<TimeFrame> t_fcst_;
    std::optional<Organization> center;
    std::optional<utc_tp> from; // requested from Date
    std::optional<utc_tp> to;   // requested to Date
    std::optional<Coord> pos;   // requested position (in WGS)
    std::optional<RepresentationType> rep_t;
    std::optional<TimePeriod> time_off_;
    size_t parameter_size = 0;
    std::vector<SearchParamTableVersion> parameters_;
};

using ExtractMeteoGrib = ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB>;

namespace serialization
{   
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,ExtractMeteoGrib>{
        auto operator()(const ExtractMeteoGrib& val,std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(val,buf,val.t_fcst_,val.center,val.from,val.to,val.pos,val.rep_t,val.time_off_,val.parameters_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,ExtractMeteoGrib>{
        auto operator()(ExtractMeteoGrib& val,std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(val,buf,val.t_fcst_,val.center,val.from,val.to,val.pos,val.rep_t,val.time_off_,val.parameters_);
        }
    };

    template<>
    struct Serial_size<ExtractMeteoGrib>{
        auto operator()(const ExtractMeteoGrib& val) const noexcept{
            return serial_size(val.t_fcst_,val.center,val.from,val.to,val.pos,val.rep_t,val.time_off_,val.parameters_);
        }
    };

    template<>
    struct Min_serial_size<ExtractMeteoGrib>{
        using type = ExtractMeteoGrib;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::t_fcst_),decltype(type::center),
            decltype(type::from),decltype(type::to),decltype(type::pos),decltype(type::rep_t),
            decltype(type::time_off_),decltype(type::parameters_)>();
        }();
    };

    template<>
    struct Max_serial_size<ExtractMeteoGrib>{
        using type = ExtractMeteoGrib;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::t_fcst_),decltype(type::center),
            decltype(type::from),decltype(type::to),decltype(type::pos),decltype(type::rep_t),
            decltype(type::time_off_),decltype(type::parameters_)>();
        }();
    };
}

using ExtractForm = std::variant<std::monostate,ExtractMeteoGrib>;

namespace network{    
    template<>
    struct MessageAdditional<network::Client_MsgT::DATA_REQUEST>:public ExtractForm{
        public:
        using variant::variant;
        MessageAdditional(ErrorCode& err){

        }
        MessageAdditional(const MessageAdditional& other) = default;
        MessageAdditional(MessageAdditional&& other)=default;
        MessageAdditional() = default;
        MessageAdditional& operator=(const MessageAdditional& other) = default;
        MessageAdditional& operator=(MessageAdditional&& other) noexcept = default;
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<network::Client_MsgT::DATA_REQUEST>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER,ExtractForm>(msg,buf);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<network::Client_MsgT::DATA_REQUEST>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER,ExtractForm>(msg,buf);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<network::Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<network::Client_MsgT::DATA_REQUEST>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size<ExtractForm>(msg);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<network::Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<network::Client_MsgT::DATA_REQUEST>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<ExtractForm>();
        }();
    };

    template<>
    struct Max_serial_size<MessageAdditional<network::Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<network::Client_MsgT::DATA_REQUEST>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<ExtractForm>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>);
static_assert(serialization::deserialize_concept<false,network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>);
static_assert(serialization::serialize_concept<true,network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>);
static_assert(serialization::serialize_concept<false,network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>);