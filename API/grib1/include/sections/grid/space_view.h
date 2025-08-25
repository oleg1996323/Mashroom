#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "grid_base.h"

namespace grid{
template<>
struct GridDefinition<RepresentationType::SPACE_VIEW>:
    GridDefinitionBase<RepresentationType::SPACE_VIEW,GridModification::NONE>{
    GridDefinition() = default;
    GridDefinition(unsigned char* buffer);
    bool operator==(const GridDefinition<RepresentationType::SPACE_VIEW>& other) const{
        return GridDefinitionBase::operator==(other);
    }
    const char* print_grid_info() const;
};
}

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,grid::GridBase<SPACE_VIEW>>{
        using type = grid::GridBase<SPACE_VIEW>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y,msg.x,msg.resolutionAndComponentFlags,msg.dx,msg.dy,
                msg.xp,msg.yp,msg.scan_mode,msg.orientation_,msg.nr,msg.Xo,msg.Yo);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,grid::GridBase<SPACE_VIEW>>{
        using type = grid::GridBase<SPACE_VIEW>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y,msg.x,msg.resolutionAndComponentFlags,msg.dx,msg.dy,
                msg.xp,msg.yp,msg.scan_mode,msg.orientation_,msg.nr,msg.Xo,msg.Yo);
        }
    };

    template<>
    struct Serial_size<grid::GridBase<SPACE_VIEW>>{
        using type = grid::GridBase<SPACE_VIEW>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.nx,msg.ny,msg.y,msg.x,msg.resolutionAndComponentFlags,msg.dx,msg.dy,
                msg.xp,msg.yp,msg.scan_mode,msg.orientation_,msg.nr,msg.Xo,msg.Yo);
        }
    };

    template<>
    struct Min_serial_size<grid::GridBase<SPACE_VIEW>>{
        using type = grid::GridBase<SPACE_VIEW>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::nx),decltype(type::ny),decltype(type::y),decltype(type::x),
                decltype(type::resolutionAndComponentFlags),decltype(type::dx),decltype(type::dy),
                decltype(type::xp),decltype(type::yp),decltype(type::scan_mode),decltype(type::orientation_),
                decltype(type::nr),decltype(type::Xo),decltype(type::Yo)>();
        }();
    };

    template<>
    struct Max_serial_size<grid::GridBase<SPACE_VIEW>>{
        using type = grid::GridBase<SPACE_VIEW>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::nx),decltype(type::ny),decltype(type::y),decltype(type::x),
                decltype(type::resolutionAndComponentFlags),decltype(type::dx),decltype(type::dy),
                decltype(type::xp),decltype(type::yp),decltype(type::scan_mode),decltype(type::orientation_),
                decltype(type::nr),decltype(type::Xo),decltype(type::Yo)>();
        }();
    };
}

static_assert(serialization::Min_serial_size<std::optional<grid::GridBase<SPACE_VIEW>>>::value==sizeof(bool));
static_assert(serialization::Max_serial_size<std::optional<grid::GridBase<SPACE_VIEW>>>::value==sizeof(bool)+serialization::Max_serial_size<grid::GridBase<SPACE_VIEW>>::value);