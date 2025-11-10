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
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::nx),decltype(type::ny),decltype(type::y1),decltype(type::x1),
                decltype(type::resolutionAndComponentFlags),decltype(type::y2),decltype(type::x2),
                decltype(type::latin),decltype(type::scan_mode),decltype(type::dy),decltype(type::dx)>();
        }();
    };

    template<>
    struct Max_serial_size<grid::GridBase<MERCATOR>>{
        using type = grid::GridBase<MERCATOR>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::nx),decltype(type::ny),decltype(type::y1),decltype(type::x1),
                decltype(type::resolutionAndComponentFlags),decltype(type::y2),decltype(type::x2),
                decltype(type::latin),decltype(type::scan_mode),decltype(type::dy),decltype(type::dx)>();
        }();
    };
}

template<>
std::expected<grid::GridBase<MERCATOR>,std::exception> from_json<grid::GridBase<MERCATOR>>(const boost::json::value& val);

template<>
boost::json::value to_json(const grid::GridBase<MERCATOR>& val);

static_assert(serialization::Min_serial_size<std::optional<grid::GridBase<MERCATOR>>>::value==sizeof(bool));
static_assert(serialization::Max_serial_size<std::optional<grid::GridBase<MERCATOR>>>::value==sizeof(bool)+serialization::Max_serial_size<grid::GridBase<MERCATOR>>::value);