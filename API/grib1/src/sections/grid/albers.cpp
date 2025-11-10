#include <sections/grid/albers.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>

GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

/// @todo
/// @return Printed by text parameters
const char* GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>::print_grid_info() const{
    assert(false);//still not available
}

template<>
std::expected<grid::GridBase<ALBERS_EQUAL_AREA>,std::exception> from_json<grid::GridBase<ALBERS_EQUAL_AREA>>(const boost::json::value& val){
    if(val.is_object()){
        grid::GridBase<ALBERS_EQUAL_AREA> result;
        auto& obj = val.as_object();
        if(obj.contains("lat1"))
            result.y1 = obj.at("lat1").as_double();
        if(obj.contains("lon1"))
            result.x1 = obj.at("lon1").as_double();
        if(obj.contains("LoV"))
            result.LoV = obj.at("LoV").as_double();
        if(obj.contains("latin1"))
            result.latin1 = obj.at("latin1").as_double();
        if(obj.contains("latin2"))
            result.latin2 = obj.at("latin2").as_double();
        if(obj.contains("Dy"))
            result.Dy = obj.at("Dy").as_double();
        if(obj.contains("Dx"))
            result.Dx = obj.at("Dx").as_double();
        if(obj.contains("lat SP"))
            result.latitude_south_pole = obj.at("lat SP").as_double();
        if(obj.contains("lon SP"))
            result.longitude_south_pole = obj.at("lon SP").as_double();
        if(obj.contains("ny"))
            result.ny = obj.at("ny").as_uint64();
        if(obj.contains("nx"))
            result.ny = obj.at("nx").as_uint64();
        if(obj.contains("isSP"))
            result.is_south_pole = obj.at("isSP").as_bool();
        if(obj.contains("isBP"))
            result.is_bipolar = obj.at("isBP").as_bool();
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
    obj["lat1"].emplace_double() = val.y1;
    obj["lon1"].emplace_double() = val.x1;
    obj["LoV"].emplace_double() = val.LoV;
    obj["latin1"].emplace_double() = val.latin1;
    obj["latin2"].emplace_double() = val.latin2;
    obj["Dy"].emplace_double() = val.Dy;
    obj["Dx"].emplace_double() = val.Dx;
    obj["lat SP"].emplace_double() = val.latitude_south_pole;
    obj["lon SP"].emplace_double() = val.longitude_south_pole;
    obj["ny"].emplace_uint64() = val.ny;
    obj["nx"].emplace_uint64() = val.nx;
    obj["isSP"].emplace_bool() = val.is_south_pole;
    obj["isBP"].emplace_bool() = val.is_bipolar;
    obj["scan mode"]= to_json(val.scan_mode);
    obj["resolution component flags"]= to_json(val.scan_mode);
    return obj;
}