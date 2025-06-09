#pragma once
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include <span>
#include <network/common/utility.h>

using namespace network::utility;

template<>
struct GridDefinition<RepresentationType::GAUSSIAN>{
    int32_t y1;                    //La1 latitude of first grid point
    int32_t x1;                    //Lo1 longitude of first grid point
    int32_t y2;                    //La2 latitude of last grid point
    int32_t x2;                    //Lo2 longitude of last grid point
    uint16_t ny;                    //Ni number of points along a parallel (unsigned)
    int16_t nx;                     //Nj number of points along a meridian (signed)
    uint16_t directionIncrement;    //Di i direction increment
    uint16_t N;                     //N number of parallels between a pole and the equator
    ScanMode scan_mode;             //Scanning mode (flags see Flag/Code table 8)
    ResolutionComponentFlags resolutionAndComponentFlags; //Resolution and component flags (see Code table 7)

    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::GAUSSIAN>& other) const;
    void serialize(std::vector<char>& buf) const{
        if(network::utility::is_little_endian()){
            int32_t y1_n = std::bit_cast<decltype(y1),std::make_unsigned_t<decltype(y1)>>(htonl(std::bit_cast<std::make_unsigned_t<decltype(y1)>,decltype(y1)>(y1)));
            int32_t x1_n = std::bit_cast<decltype(x1),std::make_unsigned_t<decltype(x1)>>(htonl(std::bit_cast<std::make_unsigned_t<decltype(x1)>,decltype(x1)>(x1)));
            int32_t y2_n = std::bit_cast<decltype(y2),std::make_unsigned_t<decltype(y2)>>(htonl(std::bit_cast<std::make_unsigned_t<decltype(y2)>,decltype(y2)>(y2)));
            int32_t x2_n = std::bit_cast<decltype(x2),std::make_unsigned_t<decltype(x2)>>(htonl(std::bit_cast<std::make_unsigned_t<decltype(x2)>,decltype(x2)>(x2)));
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
    void deserialize(std::span<const char> buf){
        if(buf.size()!=serial_size())
            return;
        else{
            uint16_t offset = 0;
            if(network::utility::is_little_endian()){
                uint16_t offset = 0;
                y1 = std::bit_cast<decltype(y1),std::make_unsigned_t<decltype(y1)>>(ntohl(std::bit_cast<std::make_unsigned_t<decltype(y1)>,decltype(y1)>(*(decltype(y1)*)(buf.data()+offset))));
                offset+=sizeof(y1);
                x1 = std::bit_cast<decltype(x1),std::make_unsigned_t<decltype(x1)>>(ntohl(std::bit_cast<std::make_unsigned_t<decltype(x1)>,decltype(x1)>(*(decltype(x1)*)(buf.data()+offset))));
                offset+=sizeof(x1);
                y2 = std::bit_cast<decltype(y2),std::make_unsigned_t<decltype(y2)>>(ntohl(std::bit_cast<std::make_unsigned_t<decltype(y2)>,decltype(y2)>(*(decltype(y2)*)(buf.data()+offset))));
                offset+=sizeof(y2);
                x2 = std::bit_cast<decltype(x2),std::make_unsigned_t<decltype(x2)>>(ntohl(std::bit_cast<std::make_unsigned_t<decltype(x2)>,decltype(x2)>(*(decltype(x2)*)(buf.data()+offset))));
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
    constexpr static uint8_t serial_size(){
        return  sizeof(y1)+sizeof(x1)+sizeof(y2)+sizeof(x2)+
                sizeof(ny)+sizeof(nx)+sizeof(directionIncrement)+
                sizeof(N)+sizeof(scan_mode)+sizeof(resolutionAndComponentFlags);
    }
};
template<>
struct GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>:virtual GridDefinition<RepresentationType::GAUSSIAN>{
    int32_t yp;                     //Latitude of the southern pole in millidegrees (integer)
    int32_t xp;                     //Longitude of the southern pole in millidegrees (integer)
    double ang;                      //Angle of rotation (represented in the same way as the reference value)

    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>& other) const;
    void serialize(std::vector<char>& buf) const{
        GridDefinition<RepresentationType::GAUSSIAN>::serialize(buf);
        if(network::utility::is_little_endian()){
            int32_t yp_n = std::bit_cast<decltype(yp),std::make_unsigned_t<decltype(yp)>>(htonl(std::bit_cast<std::make_unsigned_t<decltype(yp)>,decltype(yp)>(yp)));
            int32_t xp_n = std::bit_cast<decltype(xp),std::make_unsigned_t<decltype(xp)>>(htonl(std::bit_cast<std::make_unsigned_t<decltype(xp)>,decltype(xp)>(xp)));
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
    void deserialize(std::span<const char> buf){
        using namespace network::utility;
        if(buf.size()!=GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::serial_size())
            return;
        else{
            GridDefinition<RepresentationType::GAUSSIAN>::deserialize(buf);
            uint16_t offset = GridDefinition<RepresentationType::GAUSSIAN>::serial_size();
            if(network::utility::is_little_endian()){
                yp = std::bit_cast<decltype(yp),std::make_unsigned_t<decltype(yp)>>(ntohl(std::bit_cast<std::make_unsigned_t<decltype(yp)>,decltype(yp)>(*(decltype(yp)*)(buf.data()+offset))));
                offset+=sizeof(yp);
                xp = std::bit_cast<decltype(xp),std::make_unsigned_t<decltype(xp)>>(ntohl(std::bit_cast<std::make_unsigned_t<decltype(xp)>,decltype(xp)>(*(decltype(xp)*)(buf.data()+offset))));
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

    constexpr static uint8_t serial_size(){
        return  GridDefinition<RepresentationType::GAUSSIAN>::serial_size() + sizeof(yp)+sizeof(xp)+sizeof(ang);
    }
};
template<>
struct GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>:virtual GridDefinition<RepresentationType::GAUSSIAN>{
    int32_t ysp;
    int32_t xsp;
    double s_factor;

    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const{
        GridDefinition<RepresentationType::GAUSSIAN>::serialize(buf);
        if(network::utility::is_little_endian()){
            int32_t ysp_n = std::bit_cast<decltype(ysp),std::make_unsigned_t<decltype(ysp)>>(htonl(std::bit_cast<std::make_unsigned_t<decltype(ysp)>,decltype(ysp)>(ysp)));
            int32_t xsp_n = std::bit_cast<decltype(xsp),std::make_unsigned_t<decltype(xsp)>>(htonl(std::bit_cast<std::make_unsigned_t<decltype(xsp)>,decltype(xsp)>(xsp)));
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
    void deserialize(const std::vector<char>& buf){
        using namespace network::utility;
        if(buf.size()!=GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::serial_size())
            return;
        else{
            GridDefinition<RepresentationType::GAUSSIAN>::deserialize(buf);
            uint16_t offset = GridDefinition<RepresentationType::GAUSSIAN>::serial_size();
            if(network::utility::is_little_endian()){
                ysp = std::bit_cast<decltype(ysp),std::make_unsigned_t<decltype(ysp)>>(ntohl(std::bit_cast<std::make_unsigned_t<decltype(ysp)>,decltype(ysp)>(*(decltype(ysp)*)(buf.data()+offset))));
                offset+=sizeof(ysp);
                xsp = std::bit_cast<decltype(xsp),std::make_unsigned_t<decltype(xsp)>>(ntohl(std::bit_cast<std::make_unsigned_t<decltype(xsp)>,decltype(xsp)>(*(decltype(xsp)*)(buf.data()+offset))));
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
    bool operator==(const GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>& other) const;

    constexpr static uint8_t serial_size(){
        return GridDefinition<RepresentationType::GAUSSIAN>::serial_size() + sizeof(ysp)+sizeof(xsp)+sizeof(s_factor);
    }
};
template<>
struct GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON> final:
    GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>,GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>{

    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const{
        GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::serialize(buf);
        if(network::utility::is_little_endian()){
            int32_t yp_n = std::bit_cast<decltype(yp),std::make_unsigned_t<decltype(yp)>>(htonl(std::bit_cast<std::make_unsigned_t<decltype(yp)>,decltype(yp)>(yp)));
            int32_t xp_n = std::bit_cast<decltype(xp),std::make_unsigned_t<decltype(xp)>>(htonl(std::bit_cast<std::make_unsigned_t<decltype(xp)>,decltype(xp)>(xp)));
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
    void deserialize(const std::vector<char>& buf){
        uint16_t offset = GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::serial_size();
        if(network::utility::is_little_endian()){
            yp = std::bit_cast<decltype(yp),std::make_unsigned_t<decltype(yp)>>(ntohl(std::bit_cast<std::make_unsigned_t<decltype(yp)>,decltype(yp)>(*(decltype(yp)*)(buf.data()+offset))));
            offset+=sizeof(yp);
            xp = std::bit_cast<decltype(xp),std::make_unsigned_t<decltype(xp)>>(ntohl(std::bit_cast<std::make_unsigned_t<decltype(xp)>,decltype(xp)>(*(decltype(xp)*)(buf.data()+offset))));
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
    bool operator==(const GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>& other) const;
    constexpr static uint8_t serial_size(){
        return  GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::serial_size() + sizeof(yp)+sizeof(xp)+sizeof(ang);
    }
};