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
            if(auto y1_tmp = from_json<std::decay_t<decltype(result.y1)>>(obj.at("lat1"));
            y1_tmp.has_value())
                result.y1 = y1_tmp.value();
            else return std::unexpected(std::invalid_argument("\"lat1\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"lat1\" (not contained or not double)"));
        if(obj.contains("lon1"))
            if(auto x1_tmp = from_json<std::decay_t<decltype(result.x1)>>(obj.at("lon1"));
            x1_tmp.has_value())
                result.x1 = x1_tmp.value();
            else return std::unexpected(std::invalid_argument("\"lon1\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"lon1\" (not contained)"));
        if(obj.contains("lat2"))
            if(auto y2_tmp = from_json<std::decay_t<decltype(result.y2)>>(obj.at("lat2"));
            y2_tmp.has_value())
                result.y2 = y2_tmp.value();
            else return std::unexpected(std::invalid_argument("\"lat2\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"lat2\" (not contained)"));
        if(obj.contains("lon2"))
            if(auto x2_tmp = from_json<std::decay_t<decltype(result.x2)>>(obj.at("lon2"));
            x2_tmp.has_value())
                result.x2 = x2_tmp.value();
            else return std::unexpected(std::invalid_argument("\"lon2\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"lon2\" (not contained)"));
        if(obj.contains("dx"))
            if(auto dx_tmp = from_json<std::decay_t<decltype(result.dx)>>(obj.at("dx"));
            dx_tmp.has_value())
                result.dx = dx_tmp.value();
            else return std::unexpected(std::invalid_argument("\"dx\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"dx\" (not contained)"));
        if(obj.contains("dy"))
            if(auto dy_tmp = from_json<std::decay_t<decltype(result.dy)>>(obj.at("dy"));
            dy_tmp.has_value())
                result.dy = dy_tmp.value();
            else return std::unexpected(std::invalid_argument("\"dy\" (incorrect type or empty)"));
        else return std::unexpected(std::invalid_argument("\"dy\" (not contained)"));
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
    obj["lat1"] = val.y1;
    obj["lon1"] = val.x1;
    obj["lat2"] = val.y2;
    obj["lon2"] = val.x2;
    obj["dx"]= val.dx;
    obj["dy"]= val.dy;
    obj["ny"]= val.ny;
    obj["nx"]= val.nx;
    obj["scan mode"] = to_json(val.scan_mode);
    obj["resolution component flags"] = to_json(val.resolutionAndComponentFlags);
    return obj;
}