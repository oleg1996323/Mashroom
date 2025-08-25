#include <sections/grid/lat_lon.h>
#include <sections/grid/grid.h>
#include <format>
GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>::print_grid_info() const{
    return std::format("grid {}({}) lat{}-lon{}:lat{}-lon{} dx {} dy {}",
        (int)RepresentationType::ROTATED_LAT_LON,grid_to_abbr(RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR),
        base_.x1,base_.y1,base_.x2,base_.y2,base_.dx,base_.dy).c_str();
}

GridDefinition<RepresentationType::ROTATED_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::ROTATED_LAT_LON>::print_grid_info() const{
    return std::format("grid {}({}) lat{}-lon{}:lat{}-lon{} dx {} dy {}",
        (int)RepresentationType::ROTATED_LAT_LON,grid_to_abbr(RepresentationType::ROTATED_LAT_LON),
        base_.x1,base_.y1,base_.x2,base_.y2,base_.dx,base_.dy).c_str();
}

GridDefinition<RepresentationType::STRETCHED_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::STRETCHED_LAT_LON>::print_grid_info() const{
    return std::format("grid {}({}) lat{}-lon{}:lat{}-lon{} dx {} dy {}",
        (int)RepresentationType::ROTATED_LAT_LON,grid_to_abbr(RepresentationType::STRETCHED_LAT_LON),
        base_.x1,base_.y1,base_.x2,base_.y2,base_.dx,base_.dy).c_str();
}

GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>::print_grid_info() const{
    return std::format("grid {}({}) lat{}-lon{}:lat{}-lon{} dx {} dy {}",
        (int)RepresentationType::ROTATED_LAT_LON,grid_to_abbr(RepresentationType::STRETCHED_AND_ROTATED_LAT_LON),
        base_.x1,base_.y1,base_.x2,base_.y2,base_.dx,base_.dy).c_str();
}