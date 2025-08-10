#pragma once
#include "code_tables/table_6.h"
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "types/coord.h"
#include "def.h"

#include <span>
#include "grid_base.h"

namespace grid{
/** @brief Simple latitude-longitude grid
    @note Notes: 
    1. Latitude, longitude and increments are in millidegrees.
    2. Latitude values are limited to the range 090 000; bit 1 is set to 1 to indicate south latitude.
    3. Longitude values are limited to the range 0360 000; bit 1 is set to 1 to indicate west longitude.
    4. The latitude and longitude of the last grid point and the first grid point should always be given for regular grids.
    5. Where items are not given, the appropriate octet(s) should have all bits set to 1.
    6. Three parameters define a general latitude/longitude coordinate system,
        formed by a general rotation of the sphere. One choice for these parameters is:
     a.     The geographic latitude in degrees of the southern pole of the coordinate system, thetap for example;
     b.     The geographic longitude in degrees of the southern pole of the coordinate system, lambdap for example;
     c.     The angle of rotation in degrees about the new polar axis (measured clockwise when looking from the southern
            to the northern pole) of the coordinate system, assuming the new axis to have been obtained by first rotating
            the sphere through lambdap degrees about the geographic polar axis, and then rotating through (90 + thetap)
            degrees so that the southern pole moved along the (previously rotated) Greenwich meridian.
    7. The first and last grid points may not necessarily correspond to the first and last data points, respectively, if the bit-map section is used.
    8. For data on a quasi-regular grid, in which all the rows or columns do not necessarily have the same number of grid points, either Ni (octets 7-8) or Nj (octets 9-10) and the corresponding Di (octets 24-25) or Dj (octets 26-27) shall be coded with all bits set to 1 (missing); the actual number of points along each parallel or meridian shall be coded.
    9. A quasi-regular grid is only defined for appropriate grid scanning modes. Either rows or columns, but not both simultaneously, may have variable numbers of points. The first point in each row (column) shall be positioned at the meridian (parallel) indicated by octets 11-16. The grid points shall be evenly spaced in latitude (longitude).

    /// @attention bytes 29-32: reserved
*/
template<>
struct GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>:
    GridDefinitionBase<LAT_LON_GRID_EQUIDIST_CYLINDR,GridModification::NONE>{
    GridDefinition() = default;
    GridDefinition(unsigned char* buffer);
    bool operator==(const GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>& other) const{
        return GridDefinitionBase::operator==(other);
    }
    const char* print_grid_info() const;
    bool extendable(const GridDefinition<LAT_LON_GRID_EQUIDIST_CYLINDR>& other) const noexcept{
        if(base_.dx!=other.base_.dx ||
            base_.dy!=other.base_.dy)
            return false;
        return !(base_.x1 > other.base_.x2 ||
            base_.x2 < other.base_.x1 || 
            base_.y1 > other.base_.y2||
            base_.y2 < other.base_.y1);
    }

    bool extend(const GridDefinition<LAT_LON_GRID_EQUIDIST_CYLINDR>& other) noexcept{
        if(other.base_.x1<base_.x1)
            base_.x1 = other.base_.x1;
        if(other.base_.y2<base_.y2)
            base_.y2 = other.base_.y2;
        if(other.base_.x2>base_.x2)
            base_.x2 = other.base_.x2;
        if(other.base_.y1>base_.y1)
            base_.y1 = other.base_.y1;
        return true;
    }

    bool pos_in_grid(const Coord& pos) const noexcept{
        if(pos.lat_<base_.y2 || pos.lat_>base_.y1 || pos.lon_<base_.x1 || pos.lon_>base_.x2)
            return false;
        return true;
    }
    int value_by_raw(const Coord& pos) const noexcept{
        return (pos.lon_-base_.x1)/base_.dx+base_.nx*(pos.lat_-base_.y2)/base_.dy;
    }
};

#include "ibmtofloat.h"
template<>
struct GridDefinition<RepresentationType::ROTATED_LAT_LON>:
    GridDefinitionBase<LAT_LON_GRID_EQUIDIST_CYLINDR,GridModification::ROTATION>{
    GridDefinition() = default;
    GridDefinition(unsigned char* buffer);
    bool operator==(const GridDefinition<RepresentationType::ROTATED_LAT_LON>& other) const{
        return GridDefinitionBase::operator==(other);
    }
    const char* print_grid_info() const;
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_LAT_LON>:
    GridDefinitionBase<LAT_LON_GRID_EQUIDIST_CYLINDR,GridModification::STRETCHING>{
    GridDefinition() = default;
    GridDefinition(unsigned char* buffer);
    bool operator==(const GridDefinition<RepresentationType::STRETCHED_LAT_LON>& other) const{
        return GridDefinitionBase::operator==(other);
    }
    const char* print_grid_info() const;
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>:
    GridDefinitionBase<LAT_LON_GRID_EQUIDIST_CYLINDR,GridModification::ROTATION_STRETCHING>{
    GridDefinition(unsigned char* buffer);
    bool operator==(const GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>& other) const{
        return GridDefinitionBase::operator==(other);
    }
    const char* print_grid_info() const;
};
}

#include "serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>>{
        using type = grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.y1,msg.x1,msg.y2,msg.x2,msg.dy,msg.dx,
                msg.ny,msg.nx,msg.scan_mode,msg.resolutionAndComponentFlags);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>>{
        using type = grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.y1,msg.x1,msg.y2,msg.x2,msg.dy,msg.dx,
                msg.ny,msg.nx,msg.scan_mode,msg.resolutionAndComponentFlags);
        }
    };

    template<>
    struct Serial_size<grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>>{
        using type = grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.y1,msg.x1,msg.y2,msg.x2,msg.dy,msg.dx,
                msg.ny,msg.nx,msg.scan_mode,msg.resolutionAndComponentFlags);
        }
    };

    template<>
    struct Min_serial_size<grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>>{
        using type = grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::y1),decltype(type::x1),decltype(type::y2),decltype(type::x2),decltype(type::dy),decltype(type::dx),
                decltype(type::ny),decltype(type::nx),decltype(type::scan_mode),decltype(type::resolutionAndComponentFlags)>();
        }();
    };

    template<>
    struct Max_serial_size<grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>>{
        using type = grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::y1),decltype(type::x1),decltype(type::y2),decltype(type::x2),decltype(type::dy),decltype(type::dx),
                decltype(type::ny),decltype(type::nx),decltype(type::scan_mode),decltype(type::resolutionAndComponentFlags)>();
        }();
    };
}

static_assert(serialization::Min_serial_size<std::optional<grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>>>::value==sizeof(bool));
static_assert(serialization::Max_serial_size<std::optional<grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>>>::value==sizeof(bool)+serialization::Max_serial_size<grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>>::value);