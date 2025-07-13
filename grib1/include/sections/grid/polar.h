#pragma once
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "grid_base.h"

namespace grid{
template<>
struct GridDefinition<RepresentationType::POLAR_STEREOGRAPH_PROJ>:
    GridDefinitionBase<RepresentationType::POLAR_STEREOGRAPH_PROJ,GridModification::NONE>{
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
};
}

#include "serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,grid::GridBase<POLAR_STEREOGRAPH_PROJ>>{
        using type = grid::GridBase<POLAR_STEREOGRAPH_PROJ>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.scan_mode);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,grid::GridBase<POLAR_STEREOGRAPH_PROJ>>{
        using type = grid::GridBase<POLAR_STEREOGRAPH_PROJ>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.scan_mode);
        }
    };

    template<>
    struct Serial_size<grid::GridBase<POLAR_STEREOGRAPH_PROJ>>{
        using type = grid::GridBase<POLAR_STEREOGRAPH_PROJ>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.scan_mode);
        }
    };

    template<>
    struct Min_serial_size<grid::GridBase<POLAR_STEREOGRAPH_PROJ>>{
        using type = grid::GridBase<POLAR_STEREOGRAPH_PROJ>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.scan_mode);
        }
    };

    template<>
    struct Max_serial_size<grid::GridBase<POLAR_STEREOGRAPH_PROJ>>{
        using type = grid::GridBase<POLAR_STEREOGRAPH_PROJ>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.scan_mode);
        }
    };
}