#pragma once
#include "def.h"
#include "serialization.h"

namespace grid{

    /// @brief Parameter specifying grid modifications (stretching,rotation)
    enum class GridModification{
        NONE,
        ROTATION,
        STRETCHING,
        ROTATION_STRETCHING
    };

    template<RepresentationType REP_T, GridModification MOD>
    struct GridAdditional;

    template<RepresentationType REP_T>
    struct GridAdditional<REP_T,GridModification::NONE>{
        constexpr static size_t section_size(){
            return 0;
        }
        constexpr static size_t begin_byte();

        bool operator==(const GridAdditional<REP_T,GridModification::NONE>& other) const{
            return true;
        }
        GridAdditional() = default;
        GridAdditional(unsigned char*){}
        GridAdditional(const GridAdditional&) = default;
        GridAdditional(GridAdditional&&) = default;
        GridAdditional& operator=(const GridAdditional&) = default;
        GridAdditional& operator=(GridAdditional&&) noexcept = default;
    };

    template<RepresentationType REP_T>
    struct GridAdditional<REP_T,GridModification::ROTATION>{
        /// @brief Latitude of the southern pole in millidegrees (integer)
        float yp;
        /// @brief Longitude of the southern pole in millidegrees (integer)
        float xp;
        /// @brief Angle of rotation (represented in the same way as the reference value) (ibmfloat)
        double ang;
        GridAdditional() = default;
        GridAdditional(const GridAdditional& other):yp(other.yp),xp(other.xp),ang(other.ang){}
        GridAdditional(GridAdditional&& other):yp(other.yp),xp(other.xp),ang(other.ang){}
        GridAdditional(unsigned char* buffer):
        yp(to_float(read_bytes<3,true>(&buffer[begin_byte()]))),
        xp(to_float(read_bytes<3,true>(&buffer[begin_byte()+3]))),
        ang(ibm2flt(&buffer[begin_byte()+6])){}

        constexpr static size_t section_size(){
            return 10;
        }
        constexpr static size_t begin_byte();

        bool operator==(const GridAdditional<REP_T,GridModification::ROTATION>& other) const{
            return yp==other.yp && xp == other.xp && ang == other.ang;
        }
        GridAdditional& operator=(const GridAdditional&) = default;
        GridAdditional& operator=(GridAdditional&&) noexcept = default;
    };

    template<RepresentationType REP_T>
    struct GridAdditional<REP_T,GridModification::STRETCHING>{
        /// @brief Latitude of pole of stretching in millidegrees (integer)
        float ysp;
        /// @brief Longitude of pole of stretching in millidegrees (integer)
        float xsp;
        /// @brief Stretching factor (representation as for the reference value) (ibmfloat)
        double s_factor;
        GridAdditional() = default;
        GridAdditional(const GridAdditional& other):ysp(other.ysp),xsp(other.xsp),s_factor(other.s_factor){}
        GridAdditional(GridAdditional&& other):ysp(other.ysp),xsp(other.xsp),s_factor(other.s_factor){}
        GridAdditional(unsigned char* buffer):
        ysp(to_float(read_bytes<3,true>(&buffer[begin_byte()]))),
        xsp(to_float(read_bytes<3,true>(&buffer[begin_byte()+3]))),
        s_factor(ibm2flt(&buffer[begin_byte()+6])){}

        constexpr static size_t section_size(){
            return 10;
        }
        constexpr static size_t begin_byte();

        bool operator==(const GridAdditional<REP_T,GridModification::STRETCHING>& other) const{
            return ysp==other.ysp && xsp == other.xsp && s_factor == other.s_factor;
        }
        GridAdditional& operator=(const GridAdditional&) = default;
        GridAdditional& operator=(GridAdditional&&) noexcept = default;
    };

    template<RepresentationType REP_T>
    struct GridAdditional<REP_T,GridModification::ROTATION_STRETCHING>{
        GridAdditional<REP_T,GridModification::ROTATION> rot_;
        GridAdditional<REP_T,GridModification::STRETCHING> stretch_;
        GridAdditional() = default;
        GridAdditional(const GridAdditional& other):rot_(other.rot_),stretch_(other.stretch_){}
        GridAdditional(GridAdditional&& other):rot_(std::move(other.rot_)),stretch_(std::move(other.stretch_)){}
        GridAdditional(unsigned char* buffer):
        rot_(buffer),stretch_(buffer){}

        constexpr static size_t section_size(){
            return GridAdditional<REP_T,GridModification::ROTATION>::section_size()+
            GridAdditional<REP_T,GridModification::STRETCHING>::section_size();
        }
        constexpr static size_t begin_byte();
        bool operator==(const GridAdditional<REP_T,GridModification::ROTATION_STRETCHING>& other) const{
            return 	stretch_.ysp==other.stretch_.ysp && 
                    stretch_.xsp == other.stretch_.xsp && 
                    stretch_.s_factor == other.stretch_.s_factor && 
                    rot_.yp==other.rot_.yp && 
                    rot_.xp == other.rot_.xp && 
                    rot_.ang == other.rot_.ang;
        }
        GridAdditional& operator=(const GridAdditional&) = default;
        GridAdditional& operator=(GridAdditional&&) noexcept = default;
    };
}

namespace serialization{

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Serialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::NONE>>{
        using type = grid::GridAdditional<REP,grid::GridModification::NONE>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Deserialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::NONE>>{
        using type = grid::GridAdditional<REP,grid::GridModification::NONE>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return SerializationEC::NONE;
        }
    };

    template<RepresentationType REP>
    struct Serial_size<grid::GridAdditional<REP,grid::GridModification::NONE>>{
        using type = grid::GridAdditional<REP,grid::GridModification::NONE>;
        size_t operator()(const type& msg) const noexcept{
            return 0;
        }
    };

    template<RepresentationType REP>
    struct Min_serial_size<grid::GridAdditional<REP,grid::GridModification::NONE>>{
        using type = grid::GridAdditional<REP,grid::GridModification::NONE>;
        static constexpr size_t value = []() ->size_t
        {
            return 0;
        }();
    };

    template<RepresentationType REP>
    struct Max_serial_size<grid::GridAdditional<REP,grid::GridModification::NONE>>{
        using type = grid::GridAdditional<REP,grid::GridModification::NONE>;
        static constexpr size_t value = []() ->size_t
        {
            return 0;
        }();
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Serialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::ROTATION>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.xp,msg.yp,msg.ang);
        }
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Deserialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::ROTATION>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.xp,msg.yp,msg.ang);
        }
    };

    template<RepresentationType REP>
    struct Serial_size<grid::GridAdditional<REP,grid::GridModification::ROTATION>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.xp,msg.yp,msg.ang);
        }
    };

    template<RepresentationType REP>
    struct Min_serial_size<grid::GridAdditional<REP,grid::GridModification::ROTATION>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::xp),decltype(type::yp),decltype(type::ang)>();
        }();
    };

    template<RepresentationType REP>
    struct Max_serial_size<grid::GridAdditional<REP,grid::GridModification::ROTATION>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::xp),decltype(type::yp),decltype(type::ang)>();
        }();
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Serialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::STRETCHING>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.xsp,msg.ysp,msg.s_factor);
        }
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Deserialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::STRETCHING>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.xsp,msg.ysp,msg.s_factor);
        }
    };

    template<RepresentationType REP>
    struct Serial_size<grid::GridAdditional<REP,grid::GridModification::STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::STRETCHING>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.xsp,msg.ysp,msg.s_factor);
        }
    };

    template<RepresentationType REP>
    struct Min_serial_size<grid::GridAdditional<REP,grid::GridModification::STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::STRETCHING>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::xsp),decltype(type::ysp),decltype(type::s_factor)>();
        }();
    };

    template<RepresentationType REP>
    struct Max_serial_size<grid::GridAdditional<REP,grid::GridModification::STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::STRETCHING>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::xsp),decltype(type::ysp),decltype(type::s_factor)>();
        }();
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Serialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.rot_,msg.stretch_);
        }
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Deserialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.rot_,msg.stretch_);
        }
    };

    template<RepresentationType REP>
    struct Serial_size<grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.rot_,msg.stretch_);
        }
    };

    template<RepresentationType REP>
    struct Min_serial_size<grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::rot_),decltype(type::stretch_)>();
        }();
    };

    template<RepresentationType REP>
    struct Max_serial_size<grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::rot_),decltype(type::stretch_)>();
        }();
    };
}

#include "boost_functional/json.h"

template<RepresentationType REP>
boost::json::value to_json(const grid::GridAdditional<grid::GridDefinition<REP>::type(),grid::GridModification::NONE>& val){
    boost::json::object result;
    return result;
}
template<template<RepresentationType,grid::GridModification> class GRIDADDITION, RepresentationType REP>
std::expected<GRIDADDITION<grid::GridDefinition<REP>::type(),grid::GridModification::NONE>,std::exception> from_json(const boost::json::value& val){
    GRIDADDITION<grid::GridDefinition<REP>::type(),grid::GridModification::NONE> result;
    if(val.is_object() && !val.as_object().empty())
        return result;
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<template<RepresentationType,grid::GridModification> class GRIDADDITION, RepresentationType REP>
std::expected<GRIDADDITION<grid::GridDefinition<REP>::type(),grid::GridModification::ROTATION>,std::exception> from_json(const boost::json::value& val){
    if(val.is_object()){
        GRIDADDITION<grid::GridDefinition<REP>::type(),grid::GridModification::ROTATION> result;
        auto& obj = val.as_object();
        if(obj.contains("rotation") && obj.at("rotation").is_object()){
            const boost::json::object& in_obj = val.at("rotation").as_object();
            if(in_obj.contains("SP lat") && in_obj.at("SP lat").is_int64())
                result.yp = in_obj.at("SP lat").as_int64();
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
            if(in_obj.contains("SP lon") && in_obj.at("SP lon").is_int64())
                result.yp = in_obj.at("SP lon").as_int64();
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
            if(in_obj.contains("rot angle (ibmf)") && in_obj.at("rot angle (ibmf)").is_int64())
                result.yp = in_obj.at("rot angle (ibmf)").as_int64();
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
            return result;
        }
        else return std::unexpected(std::invalid_argument("invalid JSON input"));
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<RepresentationType REP>
boost::json::value to_json(const grid::GridAdditional<grid::GridDefinition<REP>::type(),grid::GridModification::ROTATION>& val){
    boost::json::value result = boost::json::object();
    auto& obj = result.as_object();
    obj["SP lat"].emplace_int64() = val.yp;
    obj["SP lat"].emplace_int64() = val.xp;
    obj["rot angle (ibmf)"].emplace_double() = val.ang;
    return result;
}

template<template<RepresentationType,grid::GridModification> class GRIDADDITION, RepresentationType REP>
std::expected<GRIDADDITION<grid::GridDefinition<REP>::type(),grid::GridModification::STRETCHING>,std::exception> from_json(const boost::json::value& val){
    if(val.is_object()){
        GRIDADDITION<grid::GridDefinition<REP>::type(),grid::GridModification::STRETCHING> result;
            const boost::json::object& in_obj = val.at("stretching").as_object();
            if(in_obj.contains("lat pole stretch, mm") && in_obj.at("lat pole stretch, mm").is_int64())
                result.ysp = in_obj.at("SP lat").as_int64();
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
            if(in_obj.contains("lon pole stretch, mm") && in_obj.at("lon pole stretch, mm").is_int64())
                result.xsp = in_obj.at("lon pole stretch, mm").as_int64();
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
            if(in_obj.contains("stretch factor (ibmf)") && in_obj.at("stretch factor (ibmf)").is_double())
                result.s_factor = in_obj.at("stretch factor (ibmf)").as_double();
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
            return result;
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<RepresentationType REP>
boost::json::value to_json(const grid::GridAdditional<grid::GridDefinition<REP>::type(),grid::GridModification::STRETCHING>& val){
    boost::json::value result = boost::json::object();
    auto& obj = result.as_object();
    obj["lat pole stretch, mm"].emplace_int64() = val.ysp;
    obj["lon pole stretch, mm"].emplace_int64() = val.xsp;
    obj["stretch factor (ibmf)"].emplace_double() = val.s_factor;
    return result;
}

template<template<RepresentationType,grid::GridModification> class GRIDADDITION, RepresentationType REP>
std::expected<GRIDADDITION<grid::GridDefinition<REP>::type(),grid::GridModification::ROTATION_STRETCHING>,std::exception> from_json(const boost::json::value& val){
    GRIDADDITION<grid::GridDefinition<REP>::type(),grid::GridModification::STRETCHING> result;
    if(val.is_object()){
        boost::json::object& obj = val.as_object();
        if(obj.contains("rotation") && obj.at("rotation").is_structured()){
            if(auto res_rot = from_json<GRIDADDITION<grid::GridDefinition<REP>::type(),grid::GridModification::ROTATION>>(obj.at("rotation"));res_rot.has_value())
                result.rot_ = std::move(res_rot.value());
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
        }
        else return std::unexpected(std::invalid_argument("invalid JSON input"));
        if(obj.contains("stretching") && obj.at("stretching").is_structured()){
            if(auto res_str = from_json<GRIDADDITION<grid::GridDefinition<REP>::type(),grid::GridModification::STRETCHING>>(obj.at("stretching"));res_str.has_value())
                result.stretch_ = std::move(res_str.value());
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
        }
        else return std::unexpected(std::invalid_argument("invalid JSON input"));
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
    return result;
}

template<RepresentationType REP>
boost::json::value to_json(const grid::GridAdditional<grid::GridDefinition<REP>::type(),grid::GridModification::ROTATION_STRETCHING>& val){
    boost::json::value result = boost::json::object();
    boost::json::object& obj = result.as_object();
    obj.emplace("rotation",to_json(val.rot_));
    obj.emplace("stretching",to_json(val.stretch_));
    return result;
}

template<RepresentationType REP, grid::GridModification MOD>
std::expected<grid::GridAdditional<grid::GridDefinition<REP>::type(),grid::GridDefinition<REP>::modification()>,std::exception> from_json_grid_additional(const boost::json::value& val){
    return from_json<grid::GridAdditional<grid::GridDefinition<REP>::type(),grid::GridDefinition<REP>::modification()>,grid::GridDefinition<REP>::type()>(val);
}