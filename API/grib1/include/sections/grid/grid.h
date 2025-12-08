#pragma once
#include "gaussian.h"
#include "space_view.h"
#include "harmonic.h"
#include "lat_lon.h"
#include "albers.h"
#include "gnomonic.h"
#include "utm.h"
#include "mercator.h"
#include "millers.h"
#include "polar.h"
#include "polyconic.h"
#include "lambert.h"
#include "grid_base.h"
#include "sections/grid/json/json_base.h"
#include <variant>

using namespace grid;

template<typename VARIANT,typename = void>
struct GridVariantFactory: std::false_type{};

using GridVariant = std::variant<std::monostate,
    GridDefinition<LAT_LON_GRID_EQUIDIST_CYLINDR>,
    GridDefinition<ROTATED_LAT_LON>,
    GridDefinition<STRETCHED_LAT_LON>,
    GridDefinition<STRETCHED_AND_ROTATED_LAT_LON>,
    GridDefinition<GAUSSIAN>,
    GridDefinition<ROTATED_GAUSSIAN_LAT_LON>,
    GridDefinition<STRETCHED_GAUSSIAN_LAT_LON>,
    GridDefinition<STRETCHED_ROTATED_GAUSSIAN_LAT_LON>,
    GridDefinition<LAMBERT>,
    GridDefinition<OBLIQUE_LAMBERT_CONFORMAL>,
    GridDefinition<UTM>,
    GridDefinition<ALBERS_EQUAL_AREA>,
    GridDefinition<SPACE_VIEW>,
    GridDefinition<GNOMONIC>,
    GridDefinition<SPHERICAL_HARMONIC_COEFFICIENTS>,
    GridDefinition<ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>,
    GridDefinition<STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>,
    GridDefinition<STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>,
    GridDefinition<POLAR_STEREOGRAPH_PROJ>,
    GridDefinition<MILLERS_CYLINDR>,
    GridDefinition<SIMPLE_POLYCONIC>,
    GridDefinition<MERCATOR>>;

namespace grid{
constexpr std::array<int,256> enum_to_index = [](){
    std::array<int,256> map;
    std::fill(map.begin(),map.end(),-1);
    map[UNDEF_GRID]=0;
    map[LAT_LON_GRID_EQUIDIST_CYLINDR] = 1;
    map[ROTATED_LAT_LON] = 2;
    map[STRETCHED_LAT_LON] = 3;
    map[STRETCHED_AND_ROTATED_LAT_LON] = 4;
    map[GAUSSIAN] = 5;
    map[ROTATED_GAUSSIAN_LAT_LON] = 6;
    map[STRETCHED_GAUSSIAN_LAT_LON] = 7;
    map[STRETCHED_ROTATED_GAUSSIAN_LAT_LON] = 8;
    map[LAMBERT] = 9;
    map[OBLIQUE_LAMBERT_CONFORMAL] = 10;
    map[UTM] = 11;
    map[ALBERS_EQUAL_AREA] = 12;
    map[SPACE_VIEW] = 13;
    map[GNOMONIC] = 14;
    map[SPHERICAL_HARMONIC_COEFFICIENTS] = 15;
    map[ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS] = 16;
    map[STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS] = 17;
    map[STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS] = 18;
    map[POLAR_STEREOGRAPH_PROJ] = 19;
    map[MILLERS_CYLINDR] = 20;
    map[SIMPLE_POLYCONIC] = 21;
    map[MERCATOR] = 22;
    return map;
}();
}

template<template<typename... Types> typename VARIANT, typename... Types>
struct GridVariantFactory<VARIANT<Types...>>{
    using VariantType = VARIANT<Types...>;
    using ResultType = VariantType;

    template<typename... ARGS>
    static bool emplace(ResultType& result, RepresentationType index,ARGS&&... args) {
        if (index >= enum_to_index.size() || enum_to_index[index]==-1)
            return false;
        return emplaceImpl(result, enum_to_index[index], std::make_index_sequence<sizeof...(Types)>{},std::forward<ARGS>(args)...);
    }

    template <typename T>
    static constexpr size_t index() {
        size_t index = 0;
        bool found = ((std::is_same_v<T, Types> ? true : (index++, false)) || ...);
        return found ? index : std::variant_npos;
    }

private:
    template <size_t... Is,typename... ARGS>
    static bool emplaceImpl(ResultType& result, size_t index, std::index_sequence<Is...>,ARGS&&... args) noexcept{
        bool success = false;
        auto try_emplace = [&]<size_t ID>(){
            using Type = std::variant_alternative_t<ID, VariantType>;
            if(index!=ID)
                return false;
            if constexpr (sizeof...(ARGS)>0 && std::is_constructible_v<Type,ARGS...>){
                result.template emplace<ID>(std::forward<ARGS>(args)...);
                return true;
            }
            else if constexpr(sizeof...(ARGS)==0 && std::is_default_constructible_v<Type>){
                result.template emplace<ID>(std::forward<ARGS>(args)...);
                return true;
            }
            else return false;
        };
        ((!success?( success= try_emplace.template operator()<Is>()):(success=success)),...);
        return success;
    }
};

#include <array>
#include <string_view>
#include <types/rect.h>
#include <vector>
#include "variant.h"
#ifndef GRID_INFO
struct GridInfo:GridVariant{
    using factory = GridVariantFactory<GridVariant>;
    using GridVariant::GridVariant;
    using variant = GridVariant;
    GridInfo() = default;
    GridInfo(unsigned char* buffer, RepresentationType T);
    RepresentationType type() const{
        auto dtype = [](auto& item){
            if constexpr (std::is_same_v<std::decay_t<decltype(item)>,std::monostate>)
                return RepresentationType::UNDEF_GRID;
            else return item.type();
        };
        return std::visit(dtype,*this);
    }

    template<RepresentationType REP>
    bool has() const{
        return std::holds_alternative<GridDefinition<REP>>(*this);
    }
    template<typename... ARGS>
    bool emplace_by_id(RepresentationType id, ARGS&&... args) noexcept{
        return factory::emplace(*this,id,std::forward<ARGS>(args)...);
    }
    
    bool extendable_by(const GridInfo& other) const noexcept{
        if(type() != other.type())
            return false;

        auto visitor1 = [this,&other](auto& grid1) -> bool
        {
            using Grid1 = std::decay_t<decltype(grid1)>;
            if constexpr (std::is_same_v<Grid1,std::monostate>)
                return false;
            auto visitor2 = [&grid1](auto&& grid2) mutable -> bool
            {
                using Grid1 = std::decay_t<decltype(grid1)>;
                using Grid2 = std::decay_t<decltype(grid2)>;
                if constexpr (std::is_same_v<Grid2,std::monostate>)
                    return false;
                if constexpr(requires {{grid1.extendable(grid2)} -> std::same_as<bool>;}){
                    return grid1.extendable(grid2);
                }
                else return false;
            };
            return std::visit(visitor2,other);
        };
        return std::visit(visitor1,*this);
    }

    bool extend(const GridInfo& other){
        if(!extendable_by(other))
            return false;
        auto visitor1 = [this,&other](auto&& grid1) -> bool
        {
            using Grid1 = std::decay_t<decltype(grid1)>;
            if constexpr (std::is_same_v<Grid1,std::monostate>)
                return false;
            else{
                auto visitor2 = [&grid1](auto&& grid2) mutable -> bool
                {
                    using Grid1 = std::decay_t<decltype(grid1)>;
                    using Grid2 = std::decay_t<decltype(grid2)>;
                    if constexpr (std::is_same_v<Grid2,std::monostate>)
                        return false;
                    if constexpr(requires {{grid1.extend(grid2)} -> std::same_as<bool>;}){
                        return grid1.extend(grid2);
                    }
                    else return false;
                };
                return std::visit(visitor2,other);
            }
        };
        return std::visit(visitor1,*this);
    }

    bool has_grid() const{
        return !std::holds_alternative<std::monostate>(*this);
    }

    bool has_grid_type(RepresentationType rep) const{
        return enum_to_index[rep]==this->index();
    }
    
    const char* print_grid_info() const;
    bool operator==(const GridInfo& other) const;
    bool operator!=(const GridInfo& other) const;
};
ENABLE_DERIVED_VARIANT(GridInfo,GridVariant);
#endif

#include <optional>

template<>
struct std::hash<GridInfo>{
    size_t operator()(const GridInfo& data) const{
        return std::hash<uint64_t>{}((uint64_t)data.type()<<(sizeof(size_t)-sizeof(data.type())))^
        (std::hash<std::string_view>{}(reinterpret_cast<const char*>(this))>>sizeof(data.type()));
    }
};

template<>
struct std::hash<std::optional<GridInfo>>{
    size_t operator()(const std::optional<GridInfo>& data) const{
        if(data.has_value())
            return std::hash<GridInfo>{}(data.value());
        else return 0;
    }
};

bool pos_in_grid(const Coord& pos, const GridInfo& grid) noexcept;
int value_by_raw(const Coord& pos, const GridInfo& grid) noexcept;

#include <array>
#include <string_view>

std::string_view grid_to_text(RepresentationType rep_t) noexcept;
std::string_view grid_to_abbr(RepresentationType rep_t) noexcept;

std::optional<RepresentationType> abbr_to_grid(const std::ranges::range auto& input) noexcept{
    for(int i=0;i<256;++i)
        if(grid_to_abbr(static_cast<RepresentationType>(i))==input)
            return static_cast<RepresentationType>(i);
    return std::nullopt;
}

static_assert(requires {requires serialization::serialize_concept<true,GridInfo>;});
static_assert(requires {requires serialization::serialize_concept<false,GridInfo>;});
static_assert(requires {requires serialization::serialize_concept<true,std::optional<GridInfo>>;});
static_assert(requires {requires serialization::serialize_concept<false,std::optional<GridInfo>>;});
static_assert(serialization::Min_serial_size<std::optional<GridInfo>>::value==sizeof(bool));
static_assert(serialization::Max_serial_size<std::optional<GridInfo>>::value==sizeof(bool)+serialization::Max_serial_size<GridInfo>::value);
static_assert(IsStdVariant<GridInfo>);

#include "boost_functional/json.h"

template<>
std::expected<GridInfo,std::exception> from_json(const boost::json::value& val);

template<>
boost::json::value to_json(const GridInfo& val);