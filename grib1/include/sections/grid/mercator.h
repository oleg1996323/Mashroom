#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "float.h"
#include <span>
#include "grid_base.h"

namespace grid{

template<>
struct GridDefinition<RepresentationType::MERCATOR>:
    GridDefinitionBase<RepresentationType::MERCATOR,GridModification::NONE>{

    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::MERCATOR>& other) const{
        return GridDefinitionBase::operator==(other);
    }
};
}

#include "serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,grid::GridBase<MERCATOR>>{
        using type = grid::GridBase<MERCATOR>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.y2,msg.x2,
                msg.latin,msg.scan_mode,msg.dy,msg.dx);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,grid::GridBase<MERCATOR>>{
        using type = grid::GridBase<MERCATOR>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.y2,msg.x2,
                msg.latin,msg.scan_mode,msg.dy,msg.dx);
        }
    };

    template<>
    struct Serial_size<grid::GridBase<MERCATOR>>{
        using type = grid::GridBase<MERCATOR>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.y2,msg.x2,
                msg.latin,msg.scan_mode,msg.dy,msg.dx);
        }
    };

    template<>
    struct Min_serial_size<grid::GridBase<MERCATOR>>{
        using type = grid::GridBase<MERCATOR>;
        constexpr size_t operator()(const type& msg) const noexcept{
            return min_serial_size(msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.y2,msg.x2,
                msg.latin,msg.scan_mode,msg.dy,msg.dx);
        }
    };

    template<>
    struct Max_serial_size<grid::GridBase<MERCATOR>>{
        using type = grid::GridBase<MERCATOR>;
        constexpr size_t operator()(const type& msg) const noexcept{
            return max_serial_size(msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.y2,msg.x2,
                msg.latin,msg.scan_mode,msg.dy,msg.dx);
        }
    };
}