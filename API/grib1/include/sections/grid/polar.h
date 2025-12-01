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
    GridDefinition() = default;
    GridDefinition(unsigned char* buffer);
    GridDefinition(const GridDefinition& other):GridDefinitionBase(other){}
    GridDefinition(GridDefinition&& other) noexcept:GridDefinitionBase(std::move(other)){}
    GridDefinition& operator=(const GridDefinition& other){
        if(this!=&other){
            GridDefinitionBase::operator=(other);
        }
        return *this;
    }
    GridDefinition& operator=(GridDefinition&& other){
        if(this!=&other){
            GridDefinitionBase::operator=(std::move(other));
        }
        return *this;
    }
    std::string print_grid_info() const;
};
}

#include "serialization.h"

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,grid::GridBase<POLAR_STEREOGRAPH_PROJ>>{
        using type = grid::GridBase<POLAR_STEREOGRAPH_PROJ>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.scan_mode);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,grid::GridBase<POLAR_STEREOGRAPH_PROJ>>{
        using type = grid::GridBase<POLAR_STEREOGRAPH_PROJ>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.scan_mode);
        }
    };

    template<>
    struct Serial_size<grid::GridBase<POLAR_STEREOGRAPH_PROJ>>{
        using type = grid::GridBase<POLAR_STEREOGRAPH_PROJ>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.nx,msg.ny,msg.y1,msg.x1,msg.resolutionAndComponentFlags,msg.LoV,msg.dx,
                msg.dy,msg.is_south_pole,msg.scan_mode);
        }
    };

    template<>
    struct Min_serial_size<grid::GridBase<POLAR_STEREOGRAPH_PROJ>>{
        using type = grid::GridBase<POLAR_STEREOGRAPH_PROJ>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::nx),decltype(type::ny),decltype(type::y1),decltype(type::x1),
                decltype(type::resolutionAndComponentFlags),decltype(type::LoV),decltype(type::dx),
                decltype(type::dy),decltype(type::is_south_pole),decltype(type::scan_mode)>();
        }();
    };

    template<>
    struct Max_serial_size<grid::GridBase<POLAR_STEREOGRAPH_PROJ>>{
        using type = grid::GridBase<POLAR_STEREOGRAPH_PROJ>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::nx),decltype(type::ny),decltype(type::y1),decltype(type::x1),
                decltype(type::resolutionAndComponentFlags),decltype(type::LoV),decltype(type::dx),
                decltype(type::dy),decltype(type::is_south_pole),decltype(type::scan_mode)>();
        }();
    };
}

template<>
std::expected<grid::GridBase<POLAR_STEREOGRAPH_PROJ>,std::exception> from_json<grid::GridBase<POLAR_STEREOGRAPH_PROJ>>(const boost::json::value& val);

template<>
boost::json::value to_json(const grid::GridBase<POLAR_STEREOGRAPH_PROJ>& val);

static_assert(serialization::Min_serial_size<std::optional<grid::GridBase<POLAR_STEREOGRAPH_PROJ>>>::value==sizeof(bool));
static_assert(serialization::Max_serial_size<std::optional<grid::GridBase<POLAR_STEREOGRAPH_PROJ>>>::value==sizeof(bool)+serialization::Max_serial_size<grid::GridBase<POLAR_STEREOGRAPH_PROJ>>::value);