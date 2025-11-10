#pragma once
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "grid_base.h"
#include <vector>

#ifdef __cplusplus
#include <span>

namespace grid{

/** @brief Simple gaussian grid
    @note Notes: 
    1.  Latitude, longitude and increments are in millidegrees.
    2.  Latitude values are limited to the range 0-90 000; bit 1 is set to 1 to indicate south latitude.
    3.  Longitude values are limited to the range 0-360 000; bit 1 is set to 1 to indicate west longitude.
    4.  The number of parallels between a pole and the equator is used to establish the variable (Gaussian)
        spacing of the parallels; this value must always be given.
    5.  The latitude and longitude of the last grid point and the first grid point should always be given for regular grids.
    6.  Where items are not given, the appropriate octet(s) should have all bits set to 1.
    7.  See Notes (6) to (11) under Grid definition latitude/longitude grid (or equidistant cylindrical, or Plate Carrée) page I.2 Bi 8.
    8.  Quasi-regular Gaussian latitude/longitude grids are defined only for subsets of global grids containing full latitude rows (360°).
    9.  For data on a quasi-regular grid, in which all the rows do not necessarily have the same number of grid points, Ni (octets 7-8)
        and the corresponding Di (octets 24-25) shall be coded with all bits set to 1 (missing); the actual number of points along each
        parallel shall be coded.
    10. A quasi-regular Gaussian latitude/longitude grid is only defined for the grid scanning mode with consecutive points on parallels
        (bit 3 set to zero in Code table 8). The first point in each row shall be positioned at the meridian indicated by octets 14-16
        and the last shall be positioned at the meridian indicated by octets 21-23. The grid points along each parallel shall be evenly
        spaced in longitude.

    /// @attention bytes 41-42: reserved
*/
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
    const char* print_grid_info() const;    
};
#endif
}

#include "serialization.h"

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
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
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

#include "boost_functional/json.h"

template<>
std::expected<grid::GridBase<ALBERS_EQUAL_AREA>,std::exception> from_json<grid::GridBase<ALBERS_EQUAL_AREA>>(const boost::json::value& val);

template<>
boost::json::value to_json(const grid::GridBase<ALBERS_EQUAL_AREA>& val);

static_assert(serialization::Min_serial_size<std::optional<grid::GridBase<ALBERS_EQUAL_AREA>>>::value==sizeof(bool));
static_assert(serialization::Max_serial_size<std::optional<grid::GridBase<ALBERS_EQUAL_AREA>>>::value==sizeof(bool)+serialization::Max_serial_size<grid::GridBase<ALBERS_EQUAL_AREA>>::value);