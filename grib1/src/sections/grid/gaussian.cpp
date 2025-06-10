#include <sections/grid/gaussian.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>
#include "network/common/utility.h"

using namespace network::utility;

GridDefinition<RepresentationType::GAUSSIAN>::GridDefinition(unsigned char* buffer):
    y1(0.001*GDS_LatLon_La1(buffer)),
    x1(0.001*GDS_LatLon_Lo1(buffer)),
    y2(0.001*GDS_LatLon_La2(buffer)),
    x2(0.001*GDS_LatLon_Lo2(buffer)),
    nx(GDS_LatLon_nx(buffer)),
    ny(GDS_LatLon_ny(buffer)),
    directionIncrement(GDS_LatLon_dx(buffer)),
    N(GDS_Gaussian_nlat(buffer)),
    scan_mode(ScanMode(GDS_LatLon_scan(buffer))),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_LatLon_mode(buffer))){}

bool GridDefinition<RepresentationType::GAUSSIAN>::operator==(const GridDefinition<RepresentationType::GAUSSIAN>& other) const{
    return y1 == other.y1 &&
    x1 == other.x1 &&
    y2 == other.y2 &&
    x2 == other.x2 &&
    ny == other.ny &&
    nx == other.nx &&
    scan_mode == other.scan_mode &&
    resolutionAndComponentFlags == other.resolutionAndComponentFlags &&
    N == other.N &&
    directionIncrement == other.directionIncrement;
}
/// @todo
/// @return Printed by text parameters
const char* GridDefinition<RepresentationType::GAUSSIAN>::print_grid_info() const{
    assert(false);//still not available
}
void GridDefinition<RepresentationType::GAUSSIAN>::serialize(std::vector<char>& buf) const{
    if(network::utility::is_little_endian()){
        float y1_n = htonf(y1);
        float x1_n = htonf(x1);
        float y2_n = htonf(y2);
        float x2_n = htonf(x2);
        uint16_t ny_n = htons(ny);
        int16_t nx_n = std::bit_cast<decltype(nx),std::make_unsigned_t<decltype(nx)>>(htons(std::bit_cast<std::make_unsigned_t<decltype(nx)>,decltype(nx)>(nx)));
        uint16_t directionIncrement_n = htons(directionIncrement);
        uint16_t N_n = htons(N);
        buf.insert(buf.end(),(const char*)&y1_n,(const char*)(&y1_n+sizeof(y1_n)));
        buf.insert(buf.end(),(const char*)&x1_n,(const char*)(&x1_n+sizeof(x1_n)));
        buf.insert(buf.end(),(const char*)&y2_n,(const char*)(&y2_n+sizeof(y2_n)));
        buf.insert(buf.end(),(const char*)&x2_n,(const char*)(&x2_n+sizeof(x2_n)));
        buf.insert(buf.end(),(const char*)&ny_n,(const char*)(&ny_n+sizeof(ny_n)));
        buf.insert(buf.end(),(const char*)&nx_n,(const char*)(&nx_n+sizeof(nx_n)));
        buf.insert(buf.end(),(const char*)&directionIncrement_n,(const char*)(&directionIncrement_n+sizeof(directionIncrement_n)));
        buf.insert(buf.end(),(const char*)&N_n,(const char*)(&N_n+sizeof(N_n)));
        buf.insert(buf.end(),(const char*)&scan_mode,(const char*)(&scan_mode+sizeof(scan_mode)));
        buf.insert(buf.end(),(const char*)&resolutionAndComponentFlags,(const char*)(&resolutionAndComponentFlags+sizeof(resolutionAndComponentFlags)));
    }
    else{
        buf.insert(buf.end(),(const char*)&y1,(const char*)(&y1+sizeof(y1)));
        buf.insert(buf.end(),(const char*)&x1,(const char*)(&x1+sizeof(x1)));
        buf.insert(buf.end(),(const char*)&y2,(const char*)(&y2+sizeof(y2)));
        buf.insert(buf.end(),(const char*)&x2,(const char*)(&x2+sizeof(x2)));
        buf.insert(buf.end(),(const char*)&ny,(const char*)(&ny+sizeof(ny)));
        buf.insert(buf.end(),(const char*)&nx,(const char*)(&nx+sizeof(nx)));
        buf.insert(buf.end(),(const char*)&directionIncrement,(const char*)(&directionIncrement+sizeof(directionIncrement)));
        buf.insert(buf.end(),(const char*)&N,(const char*)(&N+sizeof(N)));
        buf.insert(buf.end(),(const char*)&scan_mode,(const char*)(&scan_mode+sizeof(scan_mode)));
        buf.insert(buf.end(),(const char*)&resolutionAndComponentFlags,(const char*)(&resolutionAndComponentFlags+sizeof(resolutionAndComponentFlags)));
    }
}
void GridDefinition<RepresentationType::GAUSSIAN>::deserialize(std::span<const char> buf){
    if(buf.size()!=serial_size())
        return;
    else{
        uint16_t offset = 0;
        if(network::utility::is_little_endian()){
            y1 = ntohf(*(decltype(y1)*)(buf.data()+offset));
            offset+=sizeof(y1);
            x1 = ntohf(*(decltype(x1)*)(buf.data()+offset));
            offset+=sizeof(x1);
            y2 = ntohf(*(decltype(y2)*)(buf.data()+offset));
            offset+=sizeof(y2);
            x2 = ntohf(*(decltype(x2)*)(buf.data()+offset));
            offset+=sizeof(x2);
            ny = ntohs(*(decltype(ny)*)(buf.data()+offset));
            offset+=sizeof(ny);
            nx = std::bit_cast<decltype(nx),std::make_unsigned_t<decltype(nx)>>(ntohs(std::bit_cast<std::make_unsigned_t<decltype(nx)>,decltype(nx)>(*(decltype(nx)*)(buf.data()+offset))));
            offset+=sizeof(nx);
            directionIncrement = ntohs(*(decltype(directionIncrement)*)(buf.data()+offset));
            offset+=sizeof(directionIncrement);
            N = ntohs(*(decltype(N)*)(buf.data()+offset));
            offset+=sizeof(N);
            scan_mode = *(ScanMode*)(buf.data()+offset);
            offset+=sizeof(ScanMode);
            resolutionAndComponentFlags = *(ResolutionComponentFlags*)(buf.data()+offset);
        }
        else{
            y1 = *(decltype(y1)*)(buf.data()+offset);
            offset+=sizeof(y1);
            x1 = *(decltype(x1)*)(buf.data()+offset);
            offset+=sizeof(x1);
            y2 = *(decltype(y2)*)(buf.data()+offset);
            offset+=sizeof(y2);
            x2 = *(decltype(x2)*)(buf.data()+offset);
            offset+=sizeof(x2);
            ny = *(decltype(ny)*)(buf.data()+offset);
            offset+=sizeof(ny);
            nx = *(decltype(nx)*)(buf.data()+offset);
            offset+=sizeof(nx);
            directionIncrement = *(decltype(directionIncrement)*)(buf.data()+offset);
            offset+=sizeof(directionIncrement);
            N = *(decltype(N)*)(buf.data()+offset);
            offset+=sizeof(N);
            scan_mode = *(ScanMode*)(buf.data()+offset);
            offset+=sizeof(ScanMode);
            resolutionAndComponentFlags = *(ResolutionComponentFlags*)(buf.data()+offset);
        }
    }
}
GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinition<RepresentationType::GAUSSIAN>(buffer),
    yp(std::bit_cast<int32_t>(read_bytes(buffer[32],buffer[33],buffer[34]))),
    xp(std::bit_cast<int32_t>(read_bytes(buffer[35],buffer[36],buffer[37]))),
    ang(ibm2flt(&buffer[38])){}

bool GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::operator==(const GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>& other) const{
    return  *static_cast<const GridDefinition<RepresentationType::GAUSSIAN>*>(this)==other &&
            yp==other.yp &&
            xp==other.xp &&
            ang==other.ang;
}
/// @todo
/// @return Printed by text parameters
const char* GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::print_grid_info() const{
    assert(false);//still not available
}
void GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::serialize(std::vector<char>& buf) const{
    GridDefinition<RepresentationType::GAUSSIAN>::serialize(buf);
    if(network::utility::is_little_endian()){
        float yp_n = htonf(yp);
        float xp_n = htonf(xp);
        double ang_n = htond(ang);
        buf.insert(buf.end(),(const char*)&yp_n,(const char*)(&yp_n+sizeof(yp_n)));
        buf.insert(buf.end(),(const char*)&xp_n,(const char*)(&xp_n+sizeof(xp_n)));
        buf.insert(buf.end(),(const char*)&ang_n,(const char*)(&ang_n+sizeof(ang_n)));
    }
    else{
        buf.insert(buf.end(),(const char*)&yp,(const char*)(&yp+sizeof(yp)));
        buf.insert(buf.end(),(const char*)&xp,(const char*)(&xp+sizeof(xp)));
        buf.insert(buf.end(),(const char*)&ang,(const char*)(&ang+sizeof(ang)));
    }
}
void GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::deserialize(std::span<const char> buf){
    using namespace network::utility;
    if(buf.size()!=GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::serial_size())
        return;
    else{
        GridDefinition<RepresentationType::GAUSSIAN>::deserialize(buf);
        uint16_t offset = GridDefinition<RepresentationType::GAUSSIAN>::serial_size();
        if(network::utility::is_little_endian()){
            yp = ntohf(*(decltype(yp)*)(buf.data()+offset));
            offset+=sizeof(yp);
            xp = ntohf(*(decltype(xp)*)(buf.data()+offset));
            offset+=sizeof(xp);
            ang = ntohd((*(decltype(ang)*)(buf.data()+offset)));
        }
        else{
            yp = *(decltype(yp)*)(buf.data()+offset);
            offset+=sizeof(yp);
            xp = *(decltype(xp)*)(buf.data()+offset);
            offset+=sizeof(xp);
            ang = *(decltype(ang)*)(buf.data()+offset);
        }
    }
}
GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinition<RepresentationType::GAUSSIAN>(buffer){}

/// @todo
/// @return Printed by text parameters
const char* GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::print_grid_info() const{
    assert(false);//still not available
}
bool GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::operator==(const GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>& other) const{
    return  *static_cast<const GridDefinition<RepresentationType::GAUSSIAN>*>(this)==other &&
            ysp==other.ysp &&
            xsp==other.xsp &&
            s_factor==other.s_factor;
}

void GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::serialize(std::vector<char>& buf) const{
    GridDefinition<RepresentationType::GAUSSIAN>::serialize(buf);
    if(network::utility::is_little_endian()){
        float ysp_n = htonf(ysp);
        float xsp_n = htonf(xsp);
        double s_factor_n = htond(s_factor);
        buf.insert(buf.end(),(const char*)&ysp_n,(const char*)(&ysp_n+sizeof(ysp_n)));
        buf.insert(buf.end(),(const char*)&xsp_n,(const char*)(&xsp_n+sizeof(xsp_n)));
        buf.insert(buf.end(),(const char*)&s_factor_n,(const char*)(&s_factor_n+sizeof(s_factor_n)));
    }
    else{
        buf.insert(buf.end(),(const char*)&ysp,(const char*)(&ysp+sizeof(ysp)));
        buf.insert(buf.end(),(const char*)&xsp,(const char*)(&xsp+sizeof(xsp)));
        buf.insert(buf.end(),(const char*)&s_factor,(const char*)(&s_factor+sizeof(s_factor)));
    }
}
void GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::deserialize(std::span<const char> buf){
    using namespace network::utility;
    if(buf.size()!=GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::serial_size())
        return;
    else{
        GridDefinition<RepresentationType::GAUSSIAN>::deserialize(buf);
        uint16_t offset = GridDefinition<RepresentationType::GAUSSIAN>::serial_size();
        if(network::utility::is_little_endian()){
            ysp = ntohf(*(decltype(ysp)*)(buf.data()+offset));
            offset+=sizeof(ysp);
            xsp = ntohf(*(decltype(xsp)*)(buf.data()+offset));
            offset+=sizeof(xsp);
            s_factor = ntohd((*(decltype(s_factor)*)(buf.data()+offset)));
        }
        else{
            ysp = *(decltype(ysp)*)(buf.data()+offset);
            offset+=sizeof(ysp);
            xsp = *(decltype(xsp)*)(buf.data()+offset);
            offset+=sizeof(xsp);
            s_factor = *(decltype(s_factor)*)(buf.data()+offset);
        }
    }
}

GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>(buffer),GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>(buffer),
    GridDefinition<RepresentationType::GAUSSIAN>(buffer){}

/// @todo
/// @return Printed by text parameters
const char* GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>::print_grid_info() const{
    assert(false);//still not available
}
bool GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>::operator==(const GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>& other) const{
    return  *static_cast<const GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>*>(this)==other && 
            yp==other.yp &&
            xp==other.xp &&
            ang==other.ang;
}

void GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>::serialize(std::vector<char>& buf) const{
    GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::serialize(buf);
    if(network::utility::is_little_endian()){
        float yp_n = htonf(yp);
        float xp_n = htonf(xp);
        double ang_n = htond(ang);
        buf.insert(buf.end(),(const char*)&yp_n,(const char*)(&yp_n+sizeof(yp_n)));
        buf.insert(buf.end(),(const char*)&xp_n,(const char*)(&xp_n+sizeof(xp_n)));
        buf.insert(buf.end(),(const char*)&ang_n,(const char*)(&ang_n+sizeof(ang_n)));
    }
    else{
        buf.insert(buf.end(),(const char*)&yp,(const char*)(&yp+sizeof(yp)));
        buf.insert(buf.end(),(const char*)&xp,(const char*)(&xp+sizeof(xp)));
        buf.insert(buf.end(),(const char*)&ang,(const char*)(&ang+sizeof(ang)));
    }
}
void GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>::deserialize(std::span<const char> buf){
    uint16_t offset = GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::serial_size();
    if(network::utility::is_little_endian()){
        yp = ntohf(*(decltype(yp)*)(buf.data()+offset));
        offset+=sizeof(yp);
        xp = ntohf(*(decltype(xp)*)(buf.data()+offset));
        offset+=sizeof(xp);
        ang = ntohd((*(decltype(ang)*)(buf.data()+offset)));
    }
    else{
        yp = *(decltype(yp)*)(buf.data()+offset);
        offset+=sizeof(yp);
        xp = *(decltype(xp)*)(buf.data()+offset);
        offset+=sizeof(xp);
        ang = *(decltype(ang)*)(buf.data()+offset);
    }
}