#include <sections/grid/gnomonic.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>
#include <stdexcept>

GridDefinition<RepresentationType::GNOMONIC>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}
const char* GridDefinition<RepresentationType::GNOMONIC>::print_grid_info() const{
    return "Gnomonic grid. Information unable";
}

template<>
std::expected<grid::GridBase<GNOMONIC>,std::exception> from_json<grid::GridBase<GNOMONIC>>(const boost::json::value& val){
    if(val.is_object()){
        grid::GridBase<GNOMONIC> result;
        return result;
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const grid::GridBase<GNOMONIC>& val){
    boost::json::object obj;
    return obj;
}