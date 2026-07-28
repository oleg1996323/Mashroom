#include <grib1/sections/grid/harmonic.h>
#include <grib1/sections/grid/grid.h>
#include <format>
#include <cassert>
#include <stdexcept>

GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

std::string GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>::print_grid_info() const{
    return boost::json::serialize(to_json(*this));
}

GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

std::string GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::print_grid_info() const{
    return boost::json::serialize(to_json(*this));
}

GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

std::string GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>::print_grid_info() const{
    return boost::json::serialize(to_json(*this));
}

GridDefinition<RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

std::string GridDefinition<RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::print_grid_info() const{
    return boost::json::serialize(to_json(*this));
}

template<>
std::expected<grid::GridBase<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>,std::exception> from_json<grid::GridBase<SPHERICAL_HARMONIC_COEFFICIENTS>>(const boost::json::value& val){
    if(val.is_object()){
        grid::GridBase<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS> result;
        auto& obj = val.as_object();
        if(obj.contains("J"))
            if(auto J_tmp = from_json<std::decay_t<decltype(result.J)>>(obj.at("J"));
            J_tmp.has_value())
                result.J = J_tmp.value();
            else return std::unexpected(std::invalid_argument("\"J\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"J\" (not contained)"));
        if(obj.contains("K"))
            if(auto K_tmp = from_json<std::decay_t<decltype(result.K)>>(obj.at("K"));
            K_tmp.has_value())
                result.K = K_tmp.value();
            else return std::unexpected(std::invalid_argument("\"K\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"K\" (not contained)"));
        if(obj.contains("M"))
            if(auto M_tmp = from_json<std::decay_t<decltype(result.M)>>(obj.at("M"));
            M_tmp.has_value())
                result.M = M_tmp.value();
            else return std::unexpected(std::invalid_argument("\"M\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"M\" (not contained)"));
        if(obj.contains("reptype")&& obj.at("reptype").is_bool())
            result.representation_type = obj.at("reptype").as_bool();
        else return std::unexpected(std::invalid_argument("\"reptype\" (not contained or not bool)"));
        if(obj.contains("spectral"))
            if(auto spectral_tmp = from_json<std::decay_t<decltype(result.rep_mode)>>(obj.at("spectral"));
            spectral_tmp.has_value())
                result.rep_mode = spectral_tmp.value();
            else return std::unexpected(std::invalid_argument("\"spectral\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"spectral\" (not contained)"));
        return result;
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const grid::GridBase<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>& val){
    boost::json::object obj;
    obj["J"] = val.J;
    obj["K"] = val.K;
    obj["M"] = val.M;
    obj["reptype"] = val.representation_type;
    obj["spectral"] = val.rep_mode;
    return obj;
}