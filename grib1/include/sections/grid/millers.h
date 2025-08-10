#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include <span>
#include <stdexcept>
#include "grid_base.h"

namespace grid{
template<>
struct GridDefinition<RepresentationType::MILLERS_CYLINDR>:
        GridDefinitionBase<RepresentationType::MILLERS_CYLINDR,GridModification::NONE>{
    // uint16_t ny;
    // uint16_t nx;
    // uint32_t y1;
    // uint32_t x1;
    // ResolutionComponentFlags resolutionAndComponentFlags;
    // uint32_t y2;
    // uint32_t x2;
    // uint16_t dy;
    // uint16_t dx;
    // ScanMode scan_mode;
    // uint8_t reserved[3];
    GridDefinition() = default;
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::MILLERS_CYLINDR>& other) const{
        return true;
    }
};
}

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,grid::GridBase<MILLERS_CYLINDR>>{
        using type = grid::GridBase<MILLERS_CYLINDR>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,grid::GridBase<MILLERS_CYLINDR>>{
        using type = grid::GridBase<MILLERS_CYLINDR>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return SerializationEC::NONE;
        }
    };

    template<>
    struct Serial_size<grid::GridBase<MILLERS_CYLINDR>>{
        using type = grid::GridBase<MILLERS_CYLINDR>;
        size_t operator()(const type& msg) const noexcept{
            return 0;
        }
    };

    template<>
    struct Min_serial_size<grid::GridBase<MILLERS_CYLINDR>>{
        using type = grid::GridBase<MILLERS_CYLINDR>;
        static constexpr size_t value = []() ->size_t
        {
            return 0;
        }();
    };

    template<>
    struct Max_serial_size<grid::GridBase<MILLERS_CYLINDR>>{
        using type = grid::GridBase<MILLERS_CYLINDR>;
        static constexpr size_t value = []() ->size_t
        {
            return 0;
        }();
    };
}

static_assert(serialization::Min_serial_size<std::optional<grid::GridBase<MILLERS_CYLINDR>>>::value==sizeof(bool));
static_assert(serialization::Max_serial_size<std::optional<grid::GridBase<MILLERS_CYLINDR>>>::value==sizeof(bool)+serialization::Max_serial_size<grid::GridBase<MILLERS_CYLINDR>>::value);