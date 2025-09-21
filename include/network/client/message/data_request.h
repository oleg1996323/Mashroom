#pragma once
#include "network/common/message/msgdef.h"
#include "outputdatafileformats.h"
#include <variant>
#include <cstdint>
#include <cstddef>
#include "types/time_interval.h"
#include "properties.h"
#include "serialization.h"

template <Data::TYPE T, Data::FORMAT F>
struct ExtractRequestForm;

template <>
struct ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB>
{
    SearchProperties search_props_;
    TimePeriod t_separation_;
    OutputDataFileFormats file_fmt_;
    ExtractRequestForm() = default;
    explicit ExtractRequestForm(const SearchProperties& search_props,
        const TimePeriod& t_sep,
        OutputDataFileFormats file_fmt):
        search_props_(search_props){}
    explicit ExtractRequestForm(SearchProperties&& search_props,
        TimePeriod&& t_sep,
        OutputDataFileFormats file_fmt):search_props_(std::move(search_props)),
        t_separation_(std::move(t_sep)),
        file_fmt_(file_fmt){}
    ExtractRequestForm(const ExtractRequestForm& other) = delete;
    ExtractRequestForm(ExtractRequestForm&& other) noexcept{
        *this = std::move(other);
    }
    ExtractRequestForm& operator=(const ExtractRequestForm& other) = delete;
    ExtractRequestForm& operator=(ExtractRequestForm&& other) noexcept{
        if(this!=&other)
            search_props_ = std::move(other.search_props_);
        return *this;
    }
};

using ExtractMeteoGrib = ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB>;

namespace serialization
{   
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,ExtractMeteoGrib>{
        auto operator()(const ExtractMeteoGrib& val,std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(val,buf,val.search_props_,val.t_separation_,val.file_fmt_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,ExtractMeteoGrib>{
        auto operator()(ExtractMeteoGrib& val,std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(val,buf,val.search_props_,val.t_separation_,val.file_fmt_);
        }
    };

    template<>
    struct Serial_size<ExtractMeteoGrib>{
        auto operator()(const ExtractMeteoGrib& val) const noexcept{
            return serial_size(val.search_props_,val.t_separation_,val.file_fmt_);
        }
    };

    template<>
    struct Min_serial_size<ExtractMeteoGrib>{
        using type = ExtractMeteoGrib;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::search_props_),decltype(type::t_separation_),decltype(type::file_fmt_)>();
        }();
    };

    template<>
    struct Max_serial_size<ExtractMeteoGrib>{
        using type = ExtractMeteoGrib;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::search_props_),decltype(type::t_separation_),decltype(type::file_fmt_)>();
        }();
    };
}

using ExtractForm = std::variant<std::monostate,ExtractMeteoGrib>;

namespace network{    
    template<>
    struct MessageAdditional<network::Client_MsgT::DATA_REQUEST>{
        ExtractForm form_;
        public:
        MessageAdditional() = default;
        MessageAdditional(ErrorCode& err){}
        MessageAdditional(const MessageAdditional& other) = delete;
        MessageAdditional(MessageAdditional&& other) noexcept{
            if(this!=&other){
                *this = std::move(other);
            }
        }
        MessageAdditional(const SearchProperties& search_props,
        TimePeriod t_sep = TimePeriod(years(0),months(0),days(0),hours(0),minutes(0),std::chrono::seconds(0)),
        OutputDataFileFormats file_fmt = OutputDataFileFormats::DEFAULT){
            form_ = std::move(ExtractMeteoGrib(search_props,t_sep,file_fmt));
        }
        MessageAdditional& operator=(const MessageAdditional& other) = delete;
        MessageAdditional& operator=(MessageAdditional&& other) noexcept{
            form_ = std::move(other.form_);
            return *this;
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<network::Client_MsgT::DATA_REQUEST>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.form_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<network::Client_MsgT::DATA_REQUEST>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.form_);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<network::Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<network::Client_MsgT::DATA_REQUEST>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.form_);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<network::Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<network::Client_MsgT::DATA_REQUEST>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::form_)>();
        }();
    };

    template<>
    struct Max_serial_size<MessageAdditional<network::Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<network::Client_MsgT::DATA_REQUEST>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::form_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>);
static_assert(serialization::deserialize_concept<false,network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>);
static_assert(serialization::serialize_concept<true,network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>);
static_assert(serialization::serialize_concept<false,network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>);
static_assert(std::is_move_constructible_v<network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>);
static_assert(std::is_move_assignable_v<network::MessageAdditional<network::Client_MsgT::DATA_REQUEST>>);