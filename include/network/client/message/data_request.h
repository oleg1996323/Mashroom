#pragma once
#include "network/common/message/msgdef.h"
#include <variant>

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
    std::optional<TimeOffset> time_off_;
    size_t parameter_size = 0;
    std::vector<SearchParamTableVersion> parameters_;
};

using ExtractMeteoGrib = ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB>;

namespace serialization
{   
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,ExtractMeteoGrib>{
        auto operator()(const ExtractMeteoGrib& val,std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(val,buf,val.type,val.format,val.t_fcst_,val.center,val.from,val.to,val.pos,val.rep_t,val.time_off_,val.parameters_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,ExtractMeteoGrib>{
        auto operator()(ExtractMeteoGrib& val,std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(val,buf,val.type,val.format,val.t_fcst_,val.center,val.from,val.to,val.pos,val.rep_t,val.time_off_,val.parameters_);
        }
    };

    template<>
    struct Serial_size<ExtractMeteoGrib>{
        auto operator()(const ExtractMeteoGrib& val) noexcept{
            return serial_size(val.type,val.format,val.t_fcst_,val.center,val.from,val.to,val.pos,val.rep_t,val.time_off_,val.parameters_);
        }
    };

    template<>
    struct Min_serial_size<ExtractMeteoGrib>{
        constexpr auto operator()(const ExtractMeteoGrib& val) noexcept{
            return min_serial_size(val.type,val.format,val.t_fcst_,val.center,val.from,val.to,val.pos,val.rep_t,val.time_off_,val.parameters_);
        }
    };

    template<>
    struct Max_serial_size<ExtractMeteoGrib>{
        constexpr auto operator()(const ExtractMeteoGrib& val) noexcept{
            return min_serial_size(val.type,val.format,val.t_fcst_,val.center,val.from,val.to,val.pos,val.rep_t,val.time_off_,val.parameters_);
        }
    };
}

using ExtractForm = std::variant<std::monostate,ExtractMeteoGrib>;

namespace serialization
{   
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,ExtractForm>{
        using type = ExtractForm;
        auto operator()(const type& val,std::vector<char>& buf) noexcept{
            switch(((int)val.index())-1){
                case 1:
                    return serialize<NETWORK_ORDER>(std::get<std::variant_alternative_t<1,ExtractForm>>(val),buf);
                default:
                    return SerializationEC::UNMATCHED_TYPE;
            }
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,ExtractForm>{
        using type = ExtractForm;
        auto operator()(type& val,std::span<const char> buf) noexcept{
            switch(((int)val.index())-1){
                case 1:
                    return deserialize<NETWORK_ORDER>(std::get<std::variant_alternative_t<1,ExtractForm>>(val),buf);
                default:
                    return SerializationEC::UNMATCHED_TYPE;
            }
        }
    };

    template<>
    struct Serial_size<ExtractForm>{
        using type = ExtractForm;
        auto operator()(const type& val) noexcept{
            switch(val.index()){
                case 1:
                    return serial_size(std::get<std::variant_alternative_t<1,ExtractForm>>(val));
                default:
                    return size_t(0);
            }
        }
    };

    template<>
    struct Min_serial_size<ExtractForm>{
        using type = ExtractForm;
        constexpr auto operator()(const type& val) noexcept{
            switch(val.index()){
                case 1:
                    return min_serial_size(std::get<std::variant_alternative_t<1,ExtractForm>>(val));
                default:
                    return size_t(0);
            }
        }
    };

    template<>
    struct Max_serial_size<ExtractForm>{
        using type = ExtractForm;
        constexpr auto operator()(const type& val) noexcept{
            switch(val.index()){
                case 1:
                    return max_serial_size(std::get<std::variant_alternative_t<1,ExtractForm>>(val));
                default:
                    return size_t(0);
            }
        }
    };
}

namespace network{    
    template<>
    class MessageAdditional<Client_MsgT::DATA_REQUEST>:public ExtractForm{
        public:
        using variant::variant;
        MessageAdditional(ErrorCode& err){

        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,MessageAdditional<Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<Client_MsgT::DATA_REQUEST>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER,ExtractForm>(msg,buf);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,MessageAdditional<Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<Client_MsgT::DATA_REQUEST>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER,ExtractForm>(msg,buf);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<Client_MsgT::DATA_REQUEST>;
        size_t operator()(const type& msg) noexcept{
            return serial_size<ExtractForm>(msg);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<Client_MsgT::DATA_REQUEST>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size<ExtractForm>(msg);
        }
    };

    template<>
    struct Max_serial_size<MessageAdditional<Client_MsgT::DATA_REQUEST>>{
        using type = MessageAdditional<Client_MsgT::DATA_REQUEST>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size<ExtractForm>(msg);
        }
    };
}