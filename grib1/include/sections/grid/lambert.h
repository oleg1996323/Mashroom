#pragma once
#include "grid_base.h"

namespace grid{
    template<>
    struct GridDefinition<RepresentationType::LAMBERT>:
        GridDefinitionBase<LAMBERT,GridModification::NONE>{
        GridDefinition(unsigned char* buffer);
        const char* print_grid_info() const;
        bool operator==(const GridDefinition<RepresentationType::LAMBERT>& other) const{
            return GridDefinitionBase::operator==(other);
        }
    };

    template<>
    struct GridDefinition<RepresentationType::OBLIQUE_LAMBERT_CONFORMAL>:
        GridDefinitionBase<LAMBERT,GridModification::NONE>{
        GridDefinition(unsigned char* buffer);
        const char* print_grid_info() const;
        bool operator==(const GridDefinition<RepresentationType::OBLIQUE_LAMBERT_CONFORMAL>& other) const{
            return GridDefinitionBase::operator==(other);
        }
    };
}

#include "serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,grid::GridBase<LAMBERT>>{
        using type = grid::GridBase<LAMBERT>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.is_bipolar,msg.scan_mode,msg.latin1,msg.latin2,msg.latitude_south_pole,msg.longitude_south_pole);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,grid::GridBase<LAMBERT>>{
        using type = grid::GridBase<LAMBERT>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.is_bipolar,msg.scan_mode,msg.latin1,msg.latin2,msg.latitude_south_pole,msg.longitude_south_pole);
        }
    };

    template<>
    struct Serial_size<grid::GridBase<LAMBERT>>{
        using type = grid::GridBase<LAMBERT>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.is_bipolar,msg.scan_mode,msg.latin1,msg.latin2,msg.latitude_south_pole,msg.longitude_south_pole);
        }
    };

    template<>
    struct Min_serial_size<grid::GridBase<LAMBERT>>{
        using type = grid::GridBase<LAMBERT>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.is_bipolar,msg.scan_mode,msg.latin1,msg.latin2,msg.latitude_south_pole,msg.longitude_south_pole);
        }
    };

    template<>
    struct Max_serial_size<grid::GridBase<LAMBERT>>{
        using type = grid::GridBase<LAMBERT>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.is_bipolar,msg.scan_mode,msg.latin1,msg.latin2,msg.latitude_south_pole,msg.longitude_south_pole);
        }
    };
}