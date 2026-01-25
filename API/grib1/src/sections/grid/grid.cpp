#include "sections/grid/grid.h"
#include "grib1_def.h"

bool GridInfo::operator==(const GridInfo& other) const{
    if(type()!=other.type())
        return false;
    else{
        auto visitor1 = [this,&other](auto&& grid1) -> bool
        {
            using Grid1 = std::decay_t<decltype(grid1)>;
            auto visitor2 = [&grid1](auto&& grid2) -> bool
            {
                using Grid1 = std::decay_t<decltype(grid1)>;
                using Grid2 = std::decay_t<decltype(grid2)>;
                if constexpr(requires {{grid1==grid2} -> std::same_as<bool>;}){
                    return grid1==grid2;
                }
                else return false;
            };
            return std::visit(visitor2,other);
        };
        return std::visit(visitor1,*this);
    }
}
bool GridInfo::operator!=(const GridInfo& other) const{
	return !(*this==other);
}

#include <cmath>
#include <iostream>
bool pos_in_grid(const Coord& pos, const GridInfo& grid) noexcept{
	auto visitor = [&](auto&& grid_val){
        if constexpr(requires {{grid_val.pos_in_grid(pos)}->std::same_as<bool>;})
            return grid_val.pos_in_grid(pos);
        else return false;
    };
    return std::visit(visitor,grid);
}

int value_by_raw(const Coord& pos, const GridInfo& grid) noexcept{
    if(is_correct_pos(&pos)){
        auto visitor = [&](auto&& grid_val){
            if constexpr(requires {{grid_val.value_by_raw(pos)}->std::same_as<int>;})
                return grid_val.value_by_raw(pos);
            else return std::numeric_limits<int>::min();
        };
        return std::visit(visitor,grid);
    }
    else return UNDEFINED;
}

GridInfo::GridInfo(unsigned char* buffer, RepresentationType T){
    if(T!=UNDEF_GRID)
        emplace_by_id(T,buffer);
}

#include <cassert>
const char* GridInfo::print_grid_info() const{
    auto visitor = [this](auto&& grid_val){
        if constexpr(requires {{grid_val.print_grid_info()}->std::same_as<const char*>;})
            return grid_val.print_grid_info();
        else return "Unknown grid";
    };
    return std::visit(visitor,*this);
}

std::optional<Lat> GridInfo::top() const{
    auto get_top = [](const auto& grid_def)
    ->std::optional<Lat>
    {
        using T = std::decay_t<decltype(grid_def)>;
        if constexpr(std::is_same_v<T,GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::ROTATED_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>>||
            std::is_same_v<T,GridDefinition<RepresentationType::GAUSSIAN>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>>){
                return !grid_def.base_.scan_mode.points_sub_j_dir?grid_def.base_.y2:grid_def.base_.y1;
            }
        else if constexpr(std::is_same_v<std::monostate,T>)
            return std::nullopt;
        else return std::nullopt;//@todo
    };
    return std::visit(get_top,*this);
}
std::optional<Lat> GridInfo::bottom() const{
    auto get_bottom = [](const auto& grid_def)
    ->std::optional<Lat>
    {
        using T = std::decay_t<decltype(grid_def)>;
        if constexpr(std::is_same_v<T,GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::ROTATED_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>>||
            std::is_same_v<T,GridDefinition<RepresentationType::GAUSSIAN>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>>){
                return grid_def.base_.scan_mode.points_sub_j_dir?grid_def.base_.y2:grid_def.base_.y1;
            }
        else if constexpr(std::is_same_v<std::monostate,T>)
            return std::nullopt;
        else return std::nullopt;//@todo
    };
    return std::visit(get_bottom,*this);
}
std::optional<Lon> GridInfo::left() const{
    auto get_left = [](const auto& grid_def)
    ->std::optional<Lat>
    {
        using T = std::decay_t<decltype(grid_def)>;
        if constexpr(std::is_same_v<T,GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::ROTATED_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>>||
            std::is_same_v<T,GridDefinition<RepresentationType::GAUSSIAN>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>>){
                return !grid_def.base_.scan_mode.points_sub_i_dir?grid_def.base_.y1:grid_def.base_.y2;
            }
        else if constexpr(std::is_same_v<std::monostate,T>)
            return std::nullopt;
        else return std::nullopt;//@todo
    };
    return std::visit(get_left,*this);
}
std::optional<Lon> GridInfo::right() const{
    auto get_right = [](const auto& grid_def)
    ->std::optional<Lat>
    {
        using T = std::decay_t<decltype(grid_def)>;
        if constexpr(std::is_same_v<T,GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::ROTATED_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>>||
            std::is_same_v<T,GridDefinition<RepresentationType::GAUSSIAN>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_LAT_LON>> ||
            std::is_same_v<T,GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>>){
                return grid_def.base_.scan_mode.points_sub_i_dir?grid_def.base_.y1:grid_def.base_.y2;
            }
        else if constexpr(std::is_same_v<std::monostate,T>)
            return std::nullopt;
        else return std::nullopt;//@todo
    };
    return std::visit(get_right,*this);
}
bool GridInfo::is_stretched() const{
    auto is_stretched = [](const auto& grid_def)->bool
    {
        using T = std::decay_t<decltype(grid_def)>;
        if constexpr(std::is_same_v<T,std::monostate>)
            return false;
        else{
            auto is = []<RepresentationType T>(const GridDefinition<T>& gd)->bool
            {
                using add_t = std::decay_t<decltype(gd.additional_)>;
                if constexpr(std::is_same_v<add_t,GridAdditional<T,grid::GridModification::STRETCHING>> ||
                    std::is_same_v<add_t,GridAdditional<T,grid::GridModification::ROTATION_STRETCHING>>)
                    return true;
                else return false;
            };
            return is(grid_def);
        }
    };
    return std::visit(is_stretched,*this);
}
bool GridInfo::is_rotated() const{
    auto is_rotated = [](const auto& grid_def)->bool
    {
        using T = std::decay_t<decltype(grid_def)>;
        if constexpr(std::is_same_v<T,std::monostate>)
            return false;
        else{
            auto is = []<RepresentationType T>(const GridDefinition<T>& gd)->bool
            {
                using add_t = std::decay_t<decltype(gd.additional_)>;
                if constexpr(std::is_same_v<add_t,GridAdditional<T,grid::GridModification::ROTATION>> ||
                    std::is_same_v<add_t,GridAdditional<T,grid::GridModification::ROTATION_STRETCHING>>)
                    return true;
                else return false;
            };
            return is(grid_def);
        }
    };
    return std::visit(is_rotated,*this);
}

std::string_view grid_to_text(RepresentationType rep_t) noexcept{
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
std::string_view grid_to_abbr(RepresentationType rep_t) noexcept{
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

#include "sections/grid/json/json_add.h"

template<>
std::expected<GridInfo,std::exception> from_json(const boost::json::value& val){
    if(val.is_object()){
        auto& obj = val.as_object();
        GridInfo result;
        if(obj.contains("representation type") && obj.contains("grid data") && 
                    obj.at("representation type").is_uint64() && obj.at("grid data").is_object()){
            if(result.emplace_by_id(static_cast<RepresentationType>(obj.at("representation type").as_uint64()))){
                auto init_grid_data = [&json_data = obj.at("grid data").as_object()](auto&& grid_data)
                {   
                    if constexpr (std::is_same_v<std::monostate,std::decay_t<decltype(grid_data)>>)
                        return;
                    else{
                        auto res_tmp = from_json_grid_definition<grid_type<std::decay_t<decltype(grid_data)>>::type>(json_data);
                            if(res_tmp.has_value())
                                grid_data = res_tmp.value();
                    }
                };
                try{
                    std::visit(init_grid_data,result);
                    return result;
                }
                catch(const std::exception& err){
                    return std::unexpected(err);
                }
            }
            else return std::unexpected(std::invalid_argument("invalid JSON input"));
        }
        else return std::unexpected(std::invalid_argument("invalid JSON input"));
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const GridInfo& val){
    boost::json::object obj;
    obj["representation type"].emplace_uint64()=static_cast<uint64_t>(val.type());
    auto to_json_variant = [&obj](auto&& grid_data){
        if constexpr (!std::is_same_v<std::monostate,std::decay_t<decltype(grid_data)>>)
            obj["grid data"] = to_json(grid_data);
        else obj["grid data"].emplace_object();
    };
    std::visit(to_json_variant,val);
    return obj;
}