#include <sections/grid/lat_lon.h>
#include <sections/grid/grid.h>
#include <format>
GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

std::string GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>::print_grid_info() const{
    return boost::json::serialize(to_json(*this));
}

GridDefinition<RepresentationType::ROTATED_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

std::string GridDefinition<RepresentationType::ROTATED_LAT_LON>::print_grid_info() const{
    return boost::json::serialize(to_json(*this));
}

GridDefinition<RepresentationType::STRETCHED_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

std::string GridDefinition<RepresentationType::STRETCHED_LAT_LON>::print_grid_info() const{
    return boost::json::serialize(to_json(*this));
}

GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

std::string GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>::print_grid_info() const{
    return boost::json::serialize(to_json(*this));
}

template<>
std::expected<grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>,std::exception> from_json<grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>>(const boost::json::value& val){
    if(val.is_object()){
        grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR> result;
        auto& obj = val.as_object();
        if(obj.contains("lat1"))
            result.y1 = obj.at("lat1").as_double();
        if(obj.contains("lon1"))
            result.x1 = obj.at("lon1").as_double();
        if(obj.contains("lat2"))
            result.y2 = obj.at("lat2").as_double();
        if(obj.contains("lon2"))
            result.x2 = obj.at("lon2").as_double();
        if(obj.contains("dx"))
            result.dx = obj.at("dx").as_uint64();
        if(obj.contains("dy"))
            result.dy = obj.at("dy").as_uint64();
        if(obj.contains("ny"))
            result.ny = obj.at("ny").as_uint64();
        if(obj.contains("nx"))
            result.ny = obj.at("nx").as_uint64();
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
boost::json::value to_json(const grid::GridBase<LAT_LON_GRID_EQUIDIST_CYLINDR>& val){
    boost::json::object obj;
    obj["lat1"].emplace_double() = val.y1;
    obj["lon1"].emplace_double() = val.x1;
    obj["lat2"].emplace_double() = val.y2;
    obj["lon2"].emplace_double() = val.x2;
    obj["dx"].emplace_uint64() = val.dx;
    obj["dy"].emplace_uint64() = val.dy;
    obj["ny"].emplace_uint64() = val.ny;
    obj["nx"].emplace_uint64() = val.nx;
    obj["scan mode"] = to_json(val.scan_mode);
    obj["resolution component flags"] = to_json(val.scan_mode);
    return obj;
}