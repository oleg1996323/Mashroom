#pragma once
#include <cstdint>
#include "grib1/code_tables.h"
#include "def.h"
#include "grid_base.h"
#include <vector>

#ifdef __cplusplus
#include <span>

namespace grid{

template<>
struct GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>:
        GridDefinitionBase<RepresentationType::ALBERS_EQUAL_AREA,GridModification::NONE>{  
    GridDefinition():GridDefinitionBase(){}
    GridDefinition(unsigned char* buffer);
    GridDefinition(const GridDefinition& other):GridDefinitionBase(other){}
    GridDefinition(GridDefinition&& other) noexcept:GridDefinitionBase(std::move(other)){}
    GridDefinition& operator=(const GridDefinition& other){
        if(this!=&other){
            GridDefinitionBase::operator=(other);
        }
        return *this;
    }
    GridDefinition& operator=(GridDefinition&& other){
        if(this!=&other){
            GridDefinitionBase::operator=(std::move(other));
        }
        return *this;
    }
    /// @todo
    /// @return Printed by text parameters
    std::string print_grid_info() const;    
};
#endif
}

#include "OsterLib/serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,grid::GridBase<ALBERS_EQUAL_AREA>>{
        using type = grid::GridBase<ALBERS_EQUAL_AREA>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.Dx,
                msg.Dy,msg.is_south_pole,msg.is_bipolar,msg.scan_mode,msg.latin1,msg.latin2,msg.latitude_south_pole,msg.longitude_south_pole);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,grid::GridBase<ALBERS_EQUAL_AREA>>{
        using type = grid::GridBase<ALBERS_EQUAL_AREA>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.Dx,
                msg.Dy,msg.is_south_pole,msg.is_bipolar,msg.scan_mode,msg.latin1,msg.latin2,msg.latitude_south_pole,msg.longitude_south_pole);
        }
    };

    template<>
    struct Serial_size<grid::GridBase<ALBERS_EQUAL_AREA>>{
        using type = grid::GridBase<ALBERS_EQUAL_AREA>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.Dx,
                msg.Dy,msg.is_south_pole,msg.is_bipolar,msg.scan_mode,msg.latin1,msg.latin2,msg.latitude_south_pole,msg.longitude_south_pole);
        }
    };

    template<>
    struct Min_serial_size<grid::GridBase<ALBERS_EQUAL_AREA>>{
        using type = grid::GridBase<ALBERS_EQUAL_AREA>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::nx),decltype(type::ny),decltype(type::y1),decltype(type::x1),decltype(type::resolutionAndComponentFlags),decltype(type::LoV),decltype(type::Dx),decltype(
                type::Dy),decltype(type::is_south_pole),decltype(type::is_bipolar),decltype(type::scan_mode),decltype(type::latin1),decltype(type::latin2),decltype(type::latitude_south_pole),decltype(type::longitude_south_pole)>();
        }();
    };

    template<>
    struct Max_serial_size<grid::GridBase<ALBERS_EQUAL_AREA>>{
        using type = grid::GridBase<ALBERS_EQUAL_AREA>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::nx),decltype(type::ny),decltype(type::y1),decltype(type::x1),decltype(type::resolutionAndComponentFlags),decltype(type::LoV),decltype(type::Dx),decltype(
                type::Dy),decltype(type::is_south_pole),decltype(type::is_bipolar),decltype(type::scan_mode),decltype(type::latin1),decltype(type::latin2),decltype(type::latitude_south_pole),decltype(type::longitude_south_pole)>();
        }();
    };
}

#include "OsterLib/boost_functional/json.h"

template<>
std::expected<grid::GridBase<ALBERS_EQUAL_AREA>,std::exception> from_json<grid::GridBase<ALBERS_EQUAL_AREA>>(const boost::json::value& val);

template<>
boost::json::value to_json(const grid::GridBase<ALBERS_EQUAL_AREA>& val);

static_assert(serialization::Min_serial_size<std::optional<grid::GridBase<ALBERS_EQUAL_AREA>>>::value==sizeof(bool));
static_assert(serialization::Max_serial_size<std::optional<grid::GridBase<ALBERS_EQUAL_AREA>>>::value==sizeof(bool)+serialization::Max_serial_size<grid::GridBase<ALBERS_EQUAL_AREA>>::value);