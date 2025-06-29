#include "sections/grid/grid.h"
#include "def.h"

bool GridInfo::operator==(const GridInfo& other) const{
    if(rep_type!=other.rep_type)
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
            return std::visit(visitor2,other.data);
        };
        return std::visit(visitor1,data);
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
    return std::visit(visitor,grid.data);
}

int value_by_raw(const Coord& pos, const GridInfo& grid) noexcept{
    if(is_correct_pos(&pos)){
        auto visitor = [&](auto&& grid_val){
            if constexpr(requires {{grid_val.value_by_raw(pos)}->std::same_as<int>;})
                return grid_val.pos_in_grid(pos);
            else return false;
        };
        return std::visit(visitor,grid.data);
    }
    else return UNDEFINED;
}

GridDataType::GridDataType(unsigned char* buffer, RepresentationType T){
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
    return std::visit(visitor,data);
}