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

template<class... Types>
struct GridVariantFactory<std::variant<Types...>>{
    using VariantType = std::variant<Types...>;
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

struct GridDataType:GridVariant
{   
    using factory = GridVariantFactory<GridVariant>;
    using variant::variant;
    GridDataType() = default;
    GridDataType(unsigned char* buffer, RepresentationType T);
    template<RepresentationType REP>
    bool has() const{
        return std::holds_alternative<GridDefinition<REP>>(*this);
    }
    template<typename... ARGS>
    bool emplace_by_id(RepresentationType id, ARGS&&... args) noexcept{
        return factory::emplace(*this,id,std::forward<ARGS>(args)...);
    }
};

#include <array>
#include <string_view>
#include <types/rect.h>
#include <vector>
#ifndef GRID_INFO
struct GridInfo{
    GridDataType data = std::monostate{};
    RepresentationType rep_type = UNDEF_GRID;
    GridInfo(GridDataType&& gdt,RepresentationType t):data(std::move(gdt)),rep_type(t){}
    GridInfo() = default;
    
    bool extendable_by(const GridInfo& other) const noexcept{
        if(rep_type != other.rep_type)
            return false;

        auto visitor1 = [this,&other](auto&& grid1) -> bool
        {
            using Grid1 = std::decay_t<decltype(grid1)>;
            if constexpr (std::is_same_v<Grid1,std::monostate>)
                return false;
            auto visitor2 = [&grid1](auto&& grid2) -> bool
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
            return std::visit(visitor2,other.data);
        };
        return std::visit(visitor1,data);
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
                auto visitor2 = [&grid1](auto&& grid2) -> bool
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
                return std::visit(visitor2,other.data);
            }
        };
        return std::visit(visitor1,data);
    }

    bool has_grid() const{
        return !std::holds_alternative<std::monostate>(data);
    }

    bool has_grid_type(RepresentationType rep) const{
        return enum_to_index[rep]==data.index();
    }
    
    const char* print_grid_info() const;
    bool operator==(const GridInfo& other) const;
    bool operator!=(const GridInfo& other) const;
};
#endif

#include <optional>

template<>
struct std::hash<GridInfo>{
    size_t operator()(const GridInfo& data) const{
        return std::hash<uint64_t>{}((uint64_t)data.rep_type<<(sizeof(size_t)-sizeof(data.rep_type)))^
        (std::hash<std::string_view>{}(reinterpret_cast<const char*>(&data.data))>>sizeof(data.rep_type));
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

constexpr std::string_view grid_to_text(RepresentationType rep_t) noexcept{
    switch(rep_t){
        case RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR:
            return "Cylindr (no modifications)";
            break;
        case RepresentationType::ROTATED_LAT_LON:
            return "Rotated Cylindr";
            break;
        case RepresentationType::STRETCHED_LAT_LON:
            return "Stretched Cylindr";
            break;
        case RepresentationType::STRETCHED_AND_ROTATED_LAT_LON:
            return "Stretched and rotated Cylindr";
            break;
        case RepresentationType::GAUSSIAN:
            return "Gaussian (no modifications)";
            break;
        case RepresentationType::ROTATED_GAUSSIAN_LAT_LON:
            return "Rotated Gaussian";
            break;
        case RepresentationType::STRETCHED_GAUSSIAN_LAT_LON:
            return "Stretched Gaussian";
            break;
        case RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON:
            return "Stretched and rotated Gaussian";
            break;
        case RepresentationType::ALBERS_EQUAL_AREA:
            return "Albers";
            break;
        case RepresentationType::GNOMONIC:
            return "Gnomonic";
            break;
        case RepresentationType::LAMBERT:
            return "Lambert";
            break;
        case RepresentationType::OBLIQUE_LAMBERT_CONFORMAL:
            return "Oblique Lambert conic";
            break;
        case RepresentationType::MERCATOR:
            return "Mercator";
            break;
        case RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS:
            return "Spherical harmonic coefficients (no modifications)";
            break;
        case RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS:
            return "Rotated spherical harmonic coefficients";
            break;
        case RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS:
            return "Stretched spherical harmonic coefficients";
            break;
        case RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS:
            return "Stretched and rotated spherical harmonic coefficients";
            break;
        case RepresentationType::MILLERS_CYLINDR:
            return "Millers";
            break;
        case RepresentationType::SIMPLE_POLYCONIC:
            return "Simple polyconic";
            break;
        case RepresentationType::POLAR_STEREOGRAPH_PROJ:
            return "Polar";
            break;
        case RepresentationType::SPACE_VIEW:
            return "Spatial";
            break;
        case RepresentationType::UTM:
            return "UTM";
            break;
        default:{
            return std::string_view();
        }
    }
}

constexpr std::string_view grid_to_abbr(RepresentationType rep_t) noexcept{
    switch(rep_t){
        case RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR:
            return "Cylindr";
            break;
        case RepresentationType::ROTATED_LAT_LON:
            return "rCylindr";
            break;
        case RepresentationType::STRETCHED_LAT_LON:
            return "sCylindr";
            break;
        case RepresentationType::STRETCHED_AND_ROTATED_LAT_LON:
            return "srCylindr";
            break;
        case RepresentationType::GAUSSIAN:
            return "Gauss";
            break;
        case RepresentationType::ROTATED_GAUSSIAN_LAT_LON:
            return "rGauss";
            break;
        case RepresentationType::STRETCHED_GAUSSIAN_LAT_LON:
            return "sGauss";
            break;
        case RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON:
            return "srGauss";
            break;
        case RepresentationType::ALBERS_EQUAL_AREA:
            return "Albers";
            break;
        case RepresentationType::GNOMONIC:
            return "Gnomonic";
            break;
        case RepresentationType::LAMBERT:
            return "Lambert";
            break;
        case RepresentationType::OBLIQUE_LAMBERT_CONFORMAL:
            return "LambertConformal";
            break;
        case RepresentationType::MERCATOR:
            return "Merc";
            break;
        case RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS:
            return "SpherHarmCoef";
            break;
        case RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS:
            return "rSpherHarmCoef";
            break;
        case RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS:
            return "sSpherHarmCoef";
            break;
        case RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS:
            return "srSpherHarmCoef";
            break;
        case RepresentationType::MILLERS_CYLINDR:
            return "Millers";
            break;
        case RepresentationType::SIMPLE_POLYCONIC:
            return "SimpPolyCon";
            break;
        case RepresentationType::POLAR_STEREOGRAPH_PROJ:
            return "Polar";
            break;
        case RepresentationType::SPACE_VIEW:
            return "Spatial";
            break;
        case RepresentationType::UTM:
            return "UTM";
            break;
        default:{
            return std::string_view();
        }
    }
}

std::optional<RepresentationType> abbr_to_grid(const std::ranges::range auto& input) noexcept{
    for(int i=0;i<256;++i)
        if(grid_to_abbr(static_cast<RepresentationType>(i))==input)
            return static_cast<RepresentationType>(i);
    return std::nullopt;
}

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,GridInfo>{
        using type = GridInfo;
        SerializationEC operator()(const type& grid_val, std::vector<char>& buf) noexcept{
            auto visitor = [&buf,&grid_val](auto&& arg){
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T,std::monostate>){
                    auto rep_type = RepresentationType::UNDEF_GRID;
                    return serialization::serialize<NETWORK_ORDER>(rep_type,buf);
                }
                else return serialization::serialize<NETWORK_ORDER>(grid_val,buf,grid_val.rep_type,arg);
            };
            return std::visit(visitor,grid_val.data);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,GridInfo>{
        using type = GridInfo;
        SerializationEC operator()(type& grid_val, std::span<const char> buf) noexcept{
            grid_val.data = std::monostate();
            RepresentationType T = UNDEF_GRID;
            if(buf.size()<min_serial_size(grid_val))
                return SerializationEC::NONE;
            deserialize<NETWORK_ORDER>(T,buf);
            grid_val.rep_type = T;
            auto visitor = [&buf,&grid_val](auto&& arg){
                using ArgType = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<ArgType,std::monostate>)
                    return serialization::SerializationEC::NONE;
                else {
                    if(auto err = serialization::deserialize<NETWORK_ORDER>(arg,buf.subspan(serial_size(grid_val.rep_type)));err!=SerializationEC::NONE){
                        grid_val.data.emplace<std::monostate>();
                        grid_val.rep_type = UNDEF_GRID;
                        return err;
                    }
                    else return err;
                }
                    
            };
            if(!grid_val.data.emplace_by_id(T)){
                grid_val.data.emplace<std::monostate>();
                grid_val.rep_type = UNDEF_GRID;
                return SerializationEC::UNMATCHED_TYPE;
            }
            SerializationEC code = std::visit(visitor,grid_val.data);
            if(code!=SerializationEC::NONE){
                grid_val.data.emplace<std::monostate>();
                grid_val.rep_type = UNDEF_GRID;
            }
            return code;
        }
    };

    template<>
    struct Serial_size<GridInfo>{
        using type = GridInfo;
        size_t operator()(const type& grid_val) noexcept{
            auto visitor = [&](auto&& arg)->size_t
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T,std::monostate>)
                    return sizeof(RepresentationType);
                else return serialization::serial_size(arg);
            };
            return std::visit(visitor,grid_val.data);
        }
    };

    template<>
    struct Min_serial_size<GridInfo>{
        using type = GridInfo;
        constexpr size_t operator()(const type& grid_val) noexcept{
            auto visitor = [&](auto&& arg)->size_t
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T,std::monostate>)
                    return sizeof(RepresentationType);
                else return serialization::min_serial_size(arg);
            };
            return std::visit(visitor,grid_val.data);
        }
    };

    template<>
    struct Max_serial_size<GridInfo>{
        using type = GridInfo;
        constexpr size_t operator()(const type& grid_val) noexcept{
            auto visitor = [&](auto&& arg)->size_t
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T,std::monostate>)
                    return sizeof(RepresentationType);
                else return serialization::max_serial_size(arg);
            };
            return std::visit(visitor,grid_val.data);
        }
    };
}

static_assert(requires {requires serialization::serialize_concept<true,GridInfo>;});
static_assert(requires {requires serialization::serialize_concept<false,GridInfo>;});
static_assert(requires {requires serialization::serialize_concept<true,std::optional<GridInfo>>;});
static_assert(requires {requires serialization::serialize_concept<false,std::optional<GridInfo>>;});