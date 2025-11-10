#include <sections/grid/utm.h>
#include <sections/grid/grid.h>
#include <format>
#include <stdexcept>
#include <cassert>

GridDefinition<RepresentationType::UTM>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::UTM>::print_grid_info() const{
    assert(false);
}

template<>
std::expected<grid::GridBase<UTM>,std::exception> from_json(const boost::json::value& val){
    if(val.is_object()){
        grid::GridBase<UTM> result;
        return result;
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const grid::GridBase<UTM>& val){
    boost::json::object obj;
    return obj;
}