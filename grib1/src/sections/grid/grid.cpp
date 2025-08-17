#include "sections/grid/grid.h"
#include "def.h"

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