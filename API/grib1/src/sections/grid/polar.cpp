#include <sections/grid/polar.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>

using namespace grid;

GridDefinition<RepresentationType::POLAR_STEREOGRAPH_PROJ>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::POLAR_STEREOGRAPH_PROJ>::print_grid_info() const{
    assert(false);
}

template<>
std::expected<grid::GridBase<POLAR_STEREOGRAPH_PROJ>,std::exception> from_json(const boost::json::value& val){
    if(val.is_object()){
        grid::GridBase<POLAR_STEREOGRAPH_PROJ> result;
        return result;
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const grid::GridBase<POLAR_STEREOGRAPH_PROJ>& val){
    boost::json::object obj;
    return obj;
}