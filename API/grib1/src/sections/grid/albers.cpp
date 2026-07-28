#include <grib1/sections/grid/albers.h>
#include <grib1/sections/grid/grid.h>
#include <format>
#include <cassert>

GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

/// @todo
/// @return Printed by text parameters
std::string GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>::print_grid_info() const{
    return boost::json::serialize(to_json(*this)).c_str();
}

template<>
std::expected<grid::GridBase<ALBERS_EQUAL_AREA>,std::exception> from_json<grid::GridBase<ALBERS_EQUAL_AREA>>(const boost::json::value& val){
    if(val.is_object()){
        grid::GridBase<ALBERS_EQUAL_AREA> result;
        auto& obj = val.as_object();
        if(obj.contains("lat1"))
            if(auto y1_tmp = from_json<std::decay_t<decltype(result.y1)>>(obj.at("lat1"));
            y1_tmp.has_value())
                result.y1 = y1_tmp.value();
            else return std::unexpected(std::invalid_argument("\"lat1\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"lat1\" (not contained)"));
        if(obj.contains("lon1"))
            if(auto x1_tmp = from_json<std::decay_t<decltype(result.x1)>>(obj.at("lon1"));
            x1_tmp.has_value())
                result.x1 = x1_tmp.value();
            else return std::unexpected(std::invalid_argument("\"lon1\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"lon1\" (not contained)"));
        if(obj.contains("LoV"))
            if(auto LoV_tmp = from_json<std::decay_t<decltype(result.LoV)>>(obj.at("LoV"));
            LoV_tmp.has_value())
                result.LoV = LoV_tmp.value();
            else return std::unexpected(std::invalid_argument("\"LoV\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"LoV\" (not contained)"));
        if(obj.contains("latin1"))
            if(auto latin1_tmp = from_json<std::decay_t<decltype(result.latin1)>>(obj.at("latin1"));
            latin1_tmp.has_value())
                result.latin1 = latin1_tmp.value();
            else return std::unexpected(std::invalid_argument("\"latin1\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"latin1\" (not contained)"));
        if(obj.contains("latin2"))
            if(auto latin2_tmp = from_json<std::decay_t<decltype(result.latin2)>>(obj.at("latin2"));
            latin2_tmp.has_value())
                result.latin2 = latin2_tmp.value();
            else return std::unexpected(std::invalid_argument("\"latin2\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"latin2\" (not contained)"));
        if(obj.contains("Dy"))
            if(auto Dy_tmp = from_json<std::decay_t<decltype(result.Dy)>>(obj.at("Dy"));
            Dy_tmp.has_value())
                result.Dy = Dy_tmp.value();
            else return std::unexpected(std::invalid_argument("\"Dy\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"Dy\" (not contained)"));
        if(obj.contains("Dx"))
            if(auto Dx_tmp = from_json<std::decay_t<decltype(result.Dx)>>(obj.at("Dx"));
            Dx_tmp.has_value())
                result.Dx = Dx_tmp.value();
            else return std::unexpected(std::invalid_argument("\"Dx\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"Dx\" (not contained)"));
        if(obj.contains("lat SP"))
            if(auto lat_SP_tmp = from_json<std::decay_t<decltype(result.latitude_south_pole)>>(obj.at("lat SP"));
            lat_SP_tmp.has_value())
                result.latitude_south_pole = lat_SP_tmp.value();
            else return std::unexpected(std::invalid_argument("\"lat SP\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"lat SP\" (not contained)"));
        if(obj.contains("lon SP"))
            if(auto lon_SP_tmp = from_json<std::decay_t<decltype(result.longitude_south_pole)>>(obj.at("lon SP"));
            lon_SP_tmp.has_value())
                result.longitude_south_pole = lon_SP_tmp.value();
            else return std::unexpected(std::invalid_argument("\"lon SP\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"lon SP\" (not contained)"));
        if(obj.contains("ny"))
            if(auto ny_tmp = from_json<std::decay_t<decltype(result.ny)>>(obj.at("ny"));
            ny_tmp.has_value())
                result.ny = ny_tmp.value();
            else return std::unexpected(std::invalid_argument("\"ny\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"ny\" (not contained)"));
        if(obj.contains("nx"))
            if(auto nx_tmp = from_json<std::decay_t<decltype(result.nx)>>(obj.at("nx"));
            nx_tmp.has_value())
                result.nx = nx_tmp.value();
            else return std::unexpected(std::invalid_argument("\"nx\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"nx\" (not contained)"));
        if(obj.contains("isSP"))
            if(auto is_SP_tmp = from_json<std::decay_t<decltype(result.is_south_pole)>>(obj.at("isSP"));
            is_SP_tmp.has_value())
                result.is_south_pole = is_SP_tmp.value();
            else return std::unexpected(std::invalid_argument("\"isSP\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"isSP\" (not contained)"));
        if(obj.contains("isBP"))
            if(auto is_BP_tmp = from_json<std::decay_t<decltype(result.is_bipolar)>>(obj.at("isBP"));
            is_BP_tmp.has_value())
                result.is_bipolar = is_BP_tmp.value();
            else return std::unexpected(std::invalid_argument("\"isBP\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"isBP\" (not contained)"));
        if(obj.contains("scan mode")){
            if(auto scan_res = from_json<ScanMode>(obj.at("scan mode"));scan_res.has_value())
                result.scan_mode = scan_res.value();
            else return std::unexpected(scan_res.error());
        }
        if(obj.contains("resolution component flags")){
            if(auto resol_res = from_json<ResolutionComponentFlags>(obj.at("resolution component flags"));resol_res.has_value())
                result.resolutionAndComponentFlags = resol_res.value();
            else std::unexpected(resol_res.error());
        }
        return result;
    }
    else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const grid::GridBase<ALBERS_EQUAL_AREA>& val){
    boost::json::object obj;
    obj["lat1"] = val.y1;
    obj["lon1"] = val.x1;
    obj["LoV"] = val.LoV;
    obj["latin1"] = val.latin1;
    obj["latin2"] = val.latin2;
    obj["Dy"] = val.Dy;
    obj["Dx"] = val.Dx;
    obj["lat SP"] = val.latitude_south_pole;
    obj["lon SP"] = val.longitude_south_pole;
    obj["ny"] = val.ny;
    obj["nx"] = val.nx;
    obj["isSP"] = val.is_south_pole;
    obj["isBP"] = val.is_bipolar;
    obj["scan mode"]= to_json(val.scan_mode);
    obj["resolution component flags"]= to_json(val.resolutionAndComponentFlags);
    return obj;
}