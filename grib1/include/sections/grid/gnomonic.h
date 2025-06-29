#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "stdexcept"
#include <cassert>
#include "grid_base.h"

namespace grid{

template<>
struct GridDefinition<RepresentationType::GNOMONIC>:
        GridDefinitionBase<RepresentationType::GNOMONIC,GridModification::NONE>{

    
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
};
}

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,grid::GridBase<GNOMONIC>>{
        using type = grid::GridBase<GNOMONIC>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,grid::GridBase<GNOMONIC>>{
        using type = grid::GridBase<GNOMONIC>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return SerializationEC::NONE;
        }
    };

    template<>
    struct Serial_size<grid::GridBase<GNOMONIC>>{
        using type = grid::GridBase<GNOMONIC>;
        size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };

    template<>
    struct Min_serial_size<grid::GridBase<GNOMONIC>>{
        using type = grid::GridBase<GNOMONIC>;
        constexpr size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };

    template<>
    struct Max_serial_size<grid::GridBase<GNOMONIC>>{
        using type = grid::GridBase<GNOMONIC>;
        constexpr size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };
}