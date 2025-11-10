#include <sections/grid/space_view.h>
#include <sections/grid/grid.h>
#include <format>
#include <stdexcept>
#include <cassert>

using namespace grid;

GridDefinition<RepresentationType::SPACE_VIEW>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}
const char* GridDefinition<RepresentationType::SPACE_VIEW>::print_grid_info() const{
    assert(false);
}

template<>
std::expected<grid::GridBase<SPACE_VIEW>,std::exception> from_json<grid::GridBase<SPACE_VIEW>>(const boost::json::value& val){
    //@todo 
    return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const grid::GridBase<SPACE_VIEW>& val){
    //@todo
    return boost::json::object();
}