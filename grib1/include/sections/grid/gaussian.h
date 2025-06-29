#pragma once
#include <cstdint>
#include <span>
#include <cmath>
#include "def.h"
#include "grid_base.h"

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

    /// @attention bytes 29-32: reserved
*/
template<>
struct GridDefinition<RepresentationType::GAUSSIAN>:
        GridDefinitionBase<RepresentationType::GAUSSIAN,GridModification::NONE>{
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::GAUSSIAN>& other) const{
        return GridDefinitionBase::operator==(other);
    }

    bool pos_in_grid(const Coord& pos) noexcept{
        if(std::fmod((double)(base_.y1-pos.lat_)/
                    ((base_.y1-base_.y2)/
                    base_.ny),1)!=0 || 
                    std::fmod((double)(base_.x2-pos.lon_)/
                    ((base_.x2-base_.x1)/
                    base_.nx),1)!=0)
            return false;
        else return true;
    }
};

template<>
struct GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>:
        GridDefinitionBase<RepresentationType::GAUSSIAN,GridModification::ROTATION>{
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>& other) const{
        return GridDefinitionBase::operator==(other);
    }
};
template<>
struct GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>:
        GridDefinitionBase<RepresentationType::GAUSSIAN,GridModification::STRETCHING>{
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>& other) const{
        return GridDefinitionBase::operator==(other);
    }
};
template<>
struct GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>:
        GridDefinitionBase<GAUSSIAN,GridModification::ROTATION_STRETCHING>{
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>& other) const{
        return GridDefinitionBase::operator==(other);
    }
};
}

#include "functional/serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,grid::GridBase<GAUSSIAN>>{
        using type = grid::GridBase<GAUSSIAN>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.y1,msg.x1,msg.y2,msg.x2,msg.ny,msg.nx,
                msg.directionIncrement,msg.N,msg.scan_mode,msg.resolutionAndComponentFlags);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,grid::GridBase<GAUSSIAN>>{
        using type = grid::GridBase<GAUSSIAN>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.y1,msg.x1,msg.y2,msg.x2,msg.ny,msg.nx,
                msg.directionIncrement,msg.N,msg.scan_mode,msg.resolutionAndComponentFlags);
        }
    };

    template<>
    struct Serial_size<grid::GridBase<GAUSSIAN>>{
        using type = grid::GridBase<GAUSSIAN>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.y1,msg.x1,msg.y2,msg.x2,msg.ny,msg.nx,
                msg.directionIncrement,msg.N,msg.scan_mode,msg.resolutionAndComponentFlags);
        }
    };

    template<>
    struct Min_serial_size<grid::GridBase<GAUSSIAN>>{
        using type = grid::GridBase<GAUSSIAN>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.y1,msg.x1,msg.y2,msg.x2,msg.ny,msg.nx,
                msg.directionIncrement,msg.N,msg.scan_mode,msg.resolutionAndComponentFlags);
        }
    };

    template<>
    struct Max_serial_size<grid::GridBase<GAUSSIAN>>{
        using type = grid::GridBase<GAUSSIAN>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.y1,msg.x1,msg.y2,msg.x2,msg.ny,msg.nx,
                msg.directionIncrement,msg.N,msg.scan_mode,msg.resolutionAndComponentFlags);
        }
    };
}