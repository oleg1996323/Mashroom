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
        static constexpr size_t value = []() ->size_t
        {
            return 0;
        }();
    };

    template<>
    struct Max_serial_size<grid::GridBase<UTM>>{
        using type = grid::GridBase<UTM>;
        static constexpr size_t value = []() ->size_t
        {
            return 0;
        }();
    };
}

template<>
std::expected<grid::GridBase<UTM>,std::exception> from_json<grid::GridBase<UTM>>(const boost::json::value& val);

template<>
boost::json::value to_json(const grid::GridBase<UTM>& val);

static_assert(serialization::Min_serial_size<std::optional<grid::GridBase<UTM>>>::value==sizeof(bool));
static_assert(serialization::Max_serial_size<std::optional<grid::GridBase<UTM>>>::value==sizeof(bool)+serialization::Max_serial_size<grid::GridBase<UTM>>::value);