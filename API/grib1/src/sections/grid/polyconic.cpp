#include <sections/grid/polyconic.h>
#include <sections/grid/grid.h>
#include <format>
#include <stdexcept>
#include <cassert>
  
using namespace grid;

GridDefinition<RepresentationType::SIMPLE_POLYCONIC>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}
std::string GridDefinition<RepresentationType::SIMPLE_POLYCONIC>::print_grid_info() const{
    return boost::json::serialize(to_json(*this));
}

template<>
std::expected<grid::GridBase<SIMPLE_POLYCONIC>,std::exception> from_json<grid::GridBase<SIMPLE_POLYCONIC>>(const boost::json::value& val){
    if(val.is_object()){
        grid::GridBase<SIMPLE_POLYCONIC> result;
        return result;
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const grid::GridBase<SIMPLE_POLYCONIC>& val){
    boost::json::object obj;
    return obj;
}