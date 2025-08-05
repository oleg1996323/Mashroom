#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "grid_base.h"

namespace grid{
template<>
struct GridDefinition<RepresentationType::UTM>:
    GridDefinitionBase<RepresentationType::UTM,GridModification::NONE>{
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::UTM>& other) const{
        return GridDefinitionBase::operator==(other);
    }
};
}

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,grid::GridBase<UTM>>{
        using type = grid::GridBase<UTM>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,grid::GridBase<UTM>>{
        using type = grid::GridBase<UTM>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return SerializationEC::NONE;
        }
    };

    template<>
    struct Serial_size<grid::GridBase<UTM>>{
        using type = grid::GridBase<UTM>;
        size_t operator()(const type& msg) const noexcept{
            return 0;
        }
    };

    template<>
    struct Min_serial_size<grid::GridBase<UTM>>{
        using type = grid::GridBase<UTM>;
        size_t operator()(const type& msg) const noexcept{
            return 0;
        }
    };

    template<>
    struct Max_serial_size<grid::GridBase<UTM>>{
        using type = grid::GridBase<UTM>;
        size_t operator()(const type& msg) const noexcept{
            return 0;
        }
    };
}