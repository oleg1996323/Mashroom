#include <sections/grid/mercator.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>

using namespace grid;

GridDefinition<RepresentationType::MERCATOR>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::MERCATOR>::print_grid_info() const{
    assert(false);
}

template<>
std::expected<grid::GridBase<MERCATOR>,std::exception> from_json<grid::GridBase<MERCATOR>>(const boost::json::value& val){
    //@todo 
    return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const grid::GridBase<MERCATOR>& val){
    //@todo
    return boost::json::object();
}