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
        GridAdditional(unsigned char*){}
    };

    template<RepresentationType REP_T>
    struct GridAdditional<REP_T,GridModification::ROTATION>{
        /// @brief Latitude of the southern pole in millidegrees (integer)
        float yp;
        /// @brief Longitude of the southern pole in millidegrees (integer)
        float xp;
        /// @brief Angle of rotation (represented in the same way as the reference value) (ibmfloat)
        double ang;
        
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
    };

    template<RepresentationType REP_T>
    struct GridAdditional<REP_T,GridModification::STRETCHING>{
        /// @brief Latitude of pole of stretching in millidegrees (integer)
        float ysp;
        /// @brief Longitude of pole of stretching in millidegrees (integer)
        float xsp;
        /// @brief Stretching factor (representation as for the reference value) (ibmfloat)
        double s_factor;
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
    };

    template<RepresentationType REP_T>
    struct GridAdditional<REP_T,GridModification::ROTATION_STRETCHING>{
        GridAdditional<REP_T,GridModification::ROTATION> rot_;
        GridAdditional<REP_T,GridModification::STRETCHING> stretch_;
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
    };
}

namespace serialization{

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Serialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::NONE>>{
        using type = grid::GridAdditional<REP,grid::GridModification::NONE>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Deserialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::NONE>>{
        using type = grid::GridAdditional<REP,grid::GridModification::NONE>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return SerializationEC::NONE;
        }
    };

    template<RepresentationType REP>
    struct Serial_size<grid::GridAdditional<REP,grid::GridModification::NONE>>{
        using type = grid::GridAdditional<REP,grid::GridModification::NONE>;
        size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };

    template<RepresentationType REP>
    struct Min_serial_size<grid::GridAdditional<REP,grid::GridModification::NONE>>{
        using type = grid::GridAdditional<REP,grid::GridModification::NONE>;
        constexpr size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };

    template<RepresentationType REP>
    struct Max_serial_size<grid::GridAdditional<REP,grid::GridModification::NONE>>{
        using type = grid::GridAdditional<REP,grid::GridModification::NONE>;
        constexpr size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Serialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::ROTATION>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.xp,msg.yp,msg.ang);
        }
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Deserialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::ROTATION>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.xp,msg.yp,msg.ang);
        }
    };

    template<RepresentationType REP>
    struct Serial_size<grid::GridAdditional<REP,grid::GridModification::ROTATION>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.xp,msg.yp,msg.ang);
        }
    };

    template<RepresentationType REP>
    struct Min_serial_size<grid::GridAdditional<REP,grid::GridModification::ROTATION>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.xp,msg.yp,msg.ang);
        }
    };

    template<RepresentationType REP>
    struct Max_serial_size<grid::GridAdditional<REP,grid::GridModification::ROTATION>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.xp,msg.yp,msg.ang);
        }
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Serialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::STRETCHING>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.xsp,msg.ysp,msg.s_factor);
        }
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Deserialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::STRETCHING>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.xsp,msg.ysp,msg.s_factor);
        }
    };

    template<RepresentationType REP>
    struct Serial_size<grid::GridAdditional<REP,grid::GridModification::STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::STRETCHING>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.xsp,msg.ysp,msg.s_factor);
        }
    };

    template<RepresentationType REP>
    struct Min_serial_size<grid::GridAdditional<REP,grid::GridModification::STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::STRETCHING>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.xsp,msg.ysp,msg.s_factor);
        }
    };

    template<RepresentationType REP>
    struct Max_serial_size<grid::GridAdditional<REP,grid::GridModification::STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::STRETCHING>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.xsp,msg.ysp,msg.s_factor);
        }
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Serialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.rot_,msg.stretch_);
        }
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Deserialize<NETWORK_ORDER,grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.rot_,msg.stretch_);
        }
    };

    template<RepresentationType REP>
    struct Serial_size<grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.rot_,msg.stretch_);
        }
    };

    template<RepresentationType REP>
    struct Min_serial_size<grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.rot_,msg.stretch_);
        }
    };

    template<RepresentationType REP>
    struct Max_serial_size<grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>>{
        using type = grid::GridAdditional<REP,grid::GridModification::ROTATION_STRETCHING>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.rot_,msg.stretch_);
        }
    };
}