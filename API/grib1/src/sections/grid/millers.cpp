#include <sections/grid/millers.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>

GridDefinition<RepresentationType::MILLERS_CYLINDR>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::MILLERS_CYLINDR>::print_grid_info() const{
    assert(false);
}

template<>
std::expected<grid::GridBase<MILLERS_CYLINDR>,std::exception> from_json<grid::GridBase<MILLERS_CYLINDR>>(const boost::json::value& val){
    if(val.is_object()){
        grid::GridBase<MILLERS_CYLINDR> result;
        return result;
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const grid::GridBase<MILLERS_CYLINDR>& val){
    boost::json::object obj;
    return obj;
}