#include <sections/grid/harmonic.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>
#include <stdexcept>

GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>::print_grid_info() const{
    assert(false);//still not available
}

GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::print_grid_info() const{
    assert(false);//still not available
}

GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>::print_grid_info() const{
    assert(false);//still not available
}

GridDefinition<RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::print_grid_info() const{
    assert(false);//still not available
}

template<>
std::expected<grid::GridBase<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>,std::exception> from_json<grid::GridBase<SPHERICAL_HARMONIC_COEFFICIENTS>>(const boost::json::value& val){
    if(val.is_object()){
        grid::GridBase<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS> result;
        auto& obj = val.as_object();
        if(obj.contains("J"))
            result.J = obj.at("J").as_uint64();
        if(obj.contains("K"))
            result.K = obj.at("K").as_uint64();
        if(obj.contains("M"))
            result.M = obj.at("M").as_uint64();
        if(obj.contains("reptype"))
            result.representation_type = obj.at("reptype").as_bool();
        if(obj.contains("spectral"))
            result.rep_mode = static_cast<Spectral>(obj.at("spectral").as_uint64());
        return result;
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const grid::GridBase<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>& val){
    boost::json::object obj;
    obj["J"].emplace_uint64() = val.J;
    obj["K"].emplace_uint64() = val.K;
    obj["M"].emplace_uint64() = val.M;
    obj["reptype"].emplace_bool() = val.representation_type;
    obj["spectral"].emplace_uint64() = val.rep_mode;
    return obj;
}