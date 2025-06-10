#include <sections/grid/albers.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>
#include "network/common/utility.h"

using namespace network::utility;

GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>::GridDefinition(unsigned char* buffer):
    nx(GDS_Lambert_nx(buffer)),
    ny(GDS_Lambert_ny(buffer)),
    y1(0.001*GDS_Lambert_La1(buffer)),
    x1(0.001*GDS_Lambert_Lo1(buffer)),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_Lambert_mode(buffer))),
    LoV(0.001*GDS_Lambert_Lov(buffer)),
    Dy(0.001*GDS_Lambert_dy(buffer)),
    Dx(0.001*GDS_Lambert_dx(buffer)),
    is_south_pole((GDS_Lambert_NP(buffer)&0b01)!=0),
    is_bipolar((GDS_Lambert_NP(buffer)&0b10)!=0),
    scan_mode(ScanMode(GDS_Lambert_scan(buffer))),
    latin1(0.001*GDS_Lambert_Latin1(buffer)),
    latin2(0.001*GDS_Lambert_Latin2(buffer)),
    latitude_south_pole(0.001*GDS_Lambert_LatSP(buffer)),
    longitude_south_pole(0.001*GDS_Lambert_LonSP(buffer)){}


/// @todo
/// @return Printed by text parameters
const char* GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>::print_grid_info() const{
    assert(false);//still not available
}


void GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>::serialize(std::vector<char>& buf) const{
    if(network::utility::is_little_endian()){
        float y1_n = htonf(y1);
        float x1_n = htonf(x1);
        float LoV_n = htonf(LoV);
        float latin1_n = htonf(latin1);
        float latin2_n = htonf(latin2);
        float Dy_n = htonf(Dy);
        float Dx_n = htonf(Dx);
        float latitude_south_pole_n = htonf(latitude_south_pole);
        float longitude_south_pole_n = htonf(longitude_south_pole);
        uint16_t ny_n = htons(ny);
        uint16_t nx_n = htons(nx);
        buf.insert(buf.end(),(const char*)&y1_n,(const char*)(&y1_n+sizeof(y1_n)));
        buf.insert(buf.end(),(const char*)&x1_n,(const char*)(&x1_n+sizeof(x1_n)));
        buf.insert(buf.end(),(const char*)&LoV_n,(const char*)(&LoV_n+sizeof(LoV_n)));
        buf.insert(buf.end(),(const char*)&latin1_n,(const char*)(&latin1_n+sizeof(latin1_n)));
        buf.insert(buf.end(),(const char*)&latin2_n,(const char*)(&latin2_n+sizeof(latin2_n)));
        buf.insert(buf.end(),(const char*)&Dy_n,(const char*)(&Dy_n+sizeof(Dy_n)));
        buf.insert(buf.end(),(const char*)&Dx_n,(const char*)(&Dx_n+sizeof(Dx_n)));
        buf.insert(buf.end(),(const char*)&latitude_south_pole_n,
                            (const char*)(&latitude_south_pole_n+
                            sizeof(latitude_south_pole_n)));
        buf.insert(buf.end(),(const char*)&longitude_south_pole_n,
                            (const char*)(&longitude_south_pole_n+
                            sizeof(longitude_south_pole_n)));
        buf.insert(buf.end(),(const char*)&ny_n,(const char*)(&ny_n+sizeof(ny_n)));
        buf.insert(buf.end(),(const char*)&nx_n,(const char*)(&nx_n+sizeof(nx_n)));
        buf.insert(buf.end(),(const char*)&is_south_pole,
                            (const char*)(&is_south_pole+
                            sizeof(is_south_pole)));
        buf.insert(buf.end(),(const char*)&is_bipolar,
                            (const char*)(&is_bipolar+
                            sizeof(is_bipolar)));
        buf.insert(buf.end(),(const char*)&scan_mode,(const char*)(&scan_mode+sizeof(scan_mode)));
        buf.insert(buf.end(),(const char*)&resolutionAndComponentFlags,(const char*)(&resolutionAndComponentFlags+sizeof(resolutionAndComponentFlags)));
    }
    else{
        buf.insert(buf.end(),(const char*)&y1,(const char*)(&y1+sizeof(y1)));
        buf.insert(buf.end(),(const char*)&x1,(const char*)(&x1+sizeof(x1)));
        buf.insert(buf.end(),(const char*)&LoV,(const char*)(&LoV+sizeof(LoV)));
        buf.insert(buf.end(),(const char*)&latin1,(const char*)(&latin1+sizeof(latin1)));
        buf.insert(buf.end(),(const char*)&latin2,(const char*)(&latin2+sizeof(latin2)));
        buf.insert(buf.end(),(const char*)&Dy,(const char*)(&Dy+sizeof(Dy)));
        buf.insert(buf.end(),(const char*)&Dx,(const char*)(&Dx+sizeof(Dx)));
        buf.insert(buf.end(),(const char*)&latitude_south_pole,
                            (const char*)(&latitude_south_pole+
                            sizeof(latitude_south_pole)));
        buf.insert(buf.end(),(const char*)&longitude_south_pole,
                            (const char*)(&longitude_south_pole+
                            sizeof(longitude_south_pole)));
        buf.insert(buf.end(),(const char*)&ny,(const char*)(&ny+sizeof(ny)));
        buf.insert(buf.end(),(const char*)&nx,(const char*)(&nx+sizeof(nx)));
        buf.insert(buf.end(),(const char*)&is_south_pole,
                            (const char*)(&is_south_pole+
                            sizeof(is_south_pole)));
        buf.insert(buf.end(),(const char*)&is_bipolar,
                            (const char*)(&is_bipolar+
                            sizeof(is_bipolar)));
        buf.insert(buf.end(),(const char*)&scan_mode,(const char*)(&scan_mode+sizeof(scan_mode)));
        buf.insert(buf.end(),(const char*)&resolutionAndComponentFlags,(const char*)(&resolutionAndComponentFlags+sizeof(resolutionAndComponentFlags)));
    }
}
void GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>::deserialize(std::span<const char> buf){
    if(buf.size()!=serial_size())
        return;
    else{
        uint16_t offset = 0;
        if(network::utility::is_little_endian()){
            y1 = ntohf(*(decltype(y1)*)(buf.data()+offset));
            offset+=sizeof(y1);
            x1 = ntohf(*(decltype(x1)*)(buf.data()+offset));
            offset+=sizeof(x1);
            LoV = ntohf(*(decltype(LoV)*)(buf.data()+offset));
            offset+=sizeof(LoV);
            latin1 = ntohf(*(decltype(latin1)*)(buf.data()+offset));
            offset+=sizeof(latin1);
            latin2 = ntohf(*(decltype(latin2)*)(buf.data()+offset));
            offset+=sizeof(ny);
            Dy = ntohf(*(decltype(Dy)*)(buf.data()+offset));
            offset+=sizeof(Dy);
            Dx = ntohf(*(decltype(Dx)*)(buf.data()+offset));
            offset+=sizeof(Dx);
            latitude_south_pole = ntohf(*(decltype(latitude_south_pole)*)(buf.data()+offset));
            offset+=sizeof(latitude_south_pole);
            longitude_south_pole = ntohf(*(decltype(longitude_south_pole)*)(buf.data()+offset));
            offset+=sizeof(longitude_south_pole);
            ny = ntohs(*(decltype(ny)*)(buf.data()+offset));
            offset+=sizeof(ny);
            nx = ntohs(*(decltype(nx)*)(buf.data()+offset));
            offset+=sizeof(nx);
            is_south_pole = *(decltype(is_south_pole)*)(buf.data()+offset);
            offset+=sizeof(is_south_pole);
            is_bipolar = *(decltype(is_bipolar)*)(buf.data()+offset);
            offset+=sizeof(is_bipolar);
            scan_mode = *(ScanMode*)(buf.data()+offset);
            offset+=sizeof(ScanMode);
            resolutionAndComponentFlags = *(ResolutionComponentFlags*)(buf.data()+offset);
        }
        else{
            y1 = *(decltype(y1)*)(buf.data()+offset);
            offset+=sizeof(y1);
            x1 = *(decltype(x1)*)(buf.data()+offset);
            offset+=sizeof(x1);
            LoV = *(decltype(LoV)*)(buf.data()+offset);
            offset+=sizeof(LoV);
            latin1 = *(decltype(latin1)*)(buf.data()+offset);
            offset+=sizeof(latin1);
            latin2 = *(decltype(latin2)*)(buf.data()+offset);
            offset+=sizeof(ny);
            Dy = *(decltype(Dy)*)(buf.data()+offset);
            offset+=sizeof(Dy);
            Dx = *(decltype(Dx)*)(buf.data()+offset);
            offset+=sizeof(Dx);
            latitude_south_pole = *(decltype(latitude_south_pole)*)(buf.data()+offset);
            offset+=sizeof(latitude_south_pole);
            longitude_south_pole = *(decltype(longitude_south_pole)*)(buf.data()+offset);
            offset+=sizeof(longitude_south_pole);
            ny = *(decltype(ny)*)(buf.data()+offset);
            offset+=sizeof(ny);
            nx = *(decltype(nx)*)(buf.data()+offset);
            offset+=sizeof(nx);
            is_south_pole = *(decltype(is_south_pole)*)(buf.data()+offset);
            offset+=sizeof(is_south_pole);
            is_bipolar = *(decltype(is_bipolar)*)(buf.data()+offset);
            offset+=sizeof(is_bipolar);
            scan_mode = *(ScanMode*)(buf.data()+offset);
            offset+=sizeof(ScanMode);
            resolutionAndComponentFlags = *(ResolutionComponentFlags*)(buf.data()+offset);
        }
    }
}