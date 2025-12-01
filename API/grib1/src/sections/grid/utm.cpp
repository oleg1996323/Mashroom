#include <sections/grid/utm.h>
#include <sections/grid/grid.h>
#include <format>
#include <stdexcept>
#include <cassert>

GridDefinition<RepresentationType::UTM>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

std::string GridDefinition<RepresentationType::UTM>::print_grid_info() const{
    return boost::json::serialize(to_json(*this));
}

template<>
std::expected<grid::GridBase<UTM>,std::exception> from_json<grid::GridBase<UTM>>(const boost::json::value& val){
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