#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "grid_base.h"

namespace grid{

template<>
struct GridDefinition<RepresentationType::SIMPLE_POLYCONIC>:
        GridDefinitionBase<RepresentationType::SIMPLE_POLYCONIC,GridModification::NONE>{    
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
};
}

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,grid::GridBase<SIMPLE_POLYCONIC>>{
        using type = grid::GridBase<SIMPLE_POLYCONIC>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,grid::GridBase<SIMPLE_POLYCONIC>>{
        using type = grid::GridBase<SIMPLE_POLYCONIC>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return SerializationEC::NONE;
        }
    };

    template<>
    struct Serial_size<grid::GridBase<SIMPLE_POLYCONIC>>{
        using type = grid::GridBase<SIMPLE_POLYCONIC>;
        size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };

    template<>
    struct Min_serial_size<grid::GridBase<SIMPLE_POLYCONIC>>{
        using type = grid::GridBase<SIMPLE_POLYCONIC>;
        constexpr size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };

    template<>
    struct Max_serial_size<grid::GridBase<SIMPLE_POLYCONIC>>{
        using type = grid::GridBase<SIMPLE_POLYCONIC>;
        constexpr size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };
}