#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include <span>
#include "code_tables/table_10.h"

/** @brief Spherical harmonic grid
    @note Notes: 
    1.  The pentagonal representation of resolution is general.
        Some common truncations are special cases of the pentagonal one: Triangular M = J = K Rhomboidal K = J + M Trapezoidal K = J, K > M
    2.  The representation type (octet 13) indicates the method used to define the norm.
    3.  The representation mode (octet 14) indicates the order of the coefficients, whether global or hemispheric data are depicted,
        and the nature of the parameter stored (symmetric or antisymmetric).
    4.  See Notes (6) to (11) under Grid definition latitude/longitude grid (or equidistant cylindrical, or Plate Carrée) page I.2 Bi 8.

    /// @attention bytes 15-32: reserved
*/
template<>
struct GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>{
    /// @brief bytes 7-8: J pentagonal resolution parameter (unsigned)
    uint16_t J;
    /// @brief bytes 9-10: K pentagonal resolution parameter (unsigned)
    uint16_t K; 
    /// @brief bytes 11-12: M pentagonal resolution parameter (unsigned)
    uint16_t M;
    /**
     * @brief byte 13: Representation type (see Code table 9)
     * @details Only accessible for ECMWF:
     * Associated Legendre Polynomials of the
     * First Kind with normalization such that
     * the integral equals 1
    */
    bool representation_type;
    /**
     * @brief byte 14: Representation mode (see Code table 10)
     * @details Accessible modes for ECMWF:
     * @addindex 
     */
    Spectral rep_mode;
    
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const{
        if(network::utility::is_little_endian()){
            uint16_t J_n = htons(J);
            uint16_t K_n = htons(K);
            uint16_t M_n = htons(M);
            buf.insert(buf.end(),(const char*)&J_n,(const char*)(&J_n+sizeof(J_n)));
            buf.insert(buf.end(),(const char*)&K_n,(const char*)(&K_n+sizeof(K_n)));
            buf.insert(buf.end(),(const char*)&M_n,(const char*)(&M_n+sizeof(M_n)));
            buf.insert(buf.end(),(const char*)&representation_type,(const char*)(&representation_type+sizeof(representation_type)));
            buf.insert(buf.end(),(const char*)&rep_mode,(const char*)(&rep_mode+sizeof(rep_mode)));
        }
        else{
            buf.insert(buf.end(),(const char*)&J,(const char*)(&J+sizeof(J)));
            buf.insert(buf.end(),(const char*)&K,(const char*)(&K+sizeof(K)));
            buf.insert(buf.end(),(const char*)&M,(const char*)(&M+sizeof(M)));
            buf.insert(buf.end(),(const char*)&representation_type,(const char*)(&representation_type+sizeof(representation_type)));
            buf.insert(buf.end(),(const char*)&rep_mode,(const char*)(&rep_mode+sizeof(rep_mode)));
        }
    }
    void deserialize(std::span<const char> buf){
        if(buf.size()!=serial_size())
            return;
        else{
            uint16_t offset = 0;
            if(network::utility::is_little_endian()){
                J = ntohs(*(decltype(J)*)(buf.data()+offset));
                offset+=sizeof(J);
                K = ntohs(*(decltype(K)*)(buf.data()+offset));
                offset+=sizeof(K);
                M = ntohs(*(decltype(M)*)(buf.data()+offset));
                offset+=sizeof(M);
                representation_type = *(decltype(representation_type)*)(buf.data()+offset);
                offset+=sizeof(representation_type);
                rep_mode = *(decltype(rep_mode)*)(buf.data()+offset);
            }
            else{
                J = *(decltype(J)*)(buf.data()+offset);
                offset+=sizeof(J);
                K = *(decltype(K)*)(buf.data()+offset);
                offset+=sizeof(K);
                M = *(decltype(M)*)(buf.data()+offset);
                offset+=sizeof(M);
                representation_type = *(decltype(representation_type)*)(buf.data()+offset);
                offset+=sizeof(representation_type);
                rep_mode = *(decltype(rep_mode)*)(buf.data()+offset);
            }
        }
    }
    /// @brief Full buffer size after serialization of GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>
    /// @return uint8_t
    constexpr static uint8_t serial_size(){
        return sizeof(J)+sizeof(K)+sizeof(M)+sizeof(representation_type)+sizeof(rep_mode);
    }
    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return 26;
    }
};

/** @brief Spherical harmonic grid
    @note Notes: 
    1.  The pentagonal representation of resolution is general.
        Some common truncations are special cases of the pentagonal one: Triangular M = J = K Rhomboidal K = J + M Trapezoidal K = J, K > M
    2.  The representation type (octet 13) indicates the method used to define the norm.
    3.  The representation mode (octet 14) indicates the order of the coefficients, whether global or hemispheric data are depicted,
        and the nature of the parameter stored (symmetric or antisymmetric).
    4.  See Notes (6) to (11) under Grid definition latitude/longitude grid (or equidistant cylindrical, or Plate Carrée) page I.2 Bi 8.

    /// @attention bytes 15-32: reserved
*/
template<>
struct GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>:
    virtual GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>
{
    /// @brief bytes 33-35: Latitude of the southern pole in millidegrees (signed integer) (should multiplied by 0.001)
    float ySoP;
    /// @brief bytes 36-38: Longitude of the southern pole in millidegrees (signed integer) (should multiplied by 0.001)
    float xSoP;
    /// @brief bytes 39-42: Angle of rotation (represented in the same way as the reference value) (ibmfloat)
    double ang;
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const{
        GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>::serialize(buf);
        if(network::utility::is_little_endian()){
            float ySoP_n = htonf(ySoP);
            float xSoP_n = htonf(xSoP);
            double ang_n = htond(ang);
            buf.insert(buf.end(),(const char*)&ySoP_n,(const char*)(&ySoP_n+sizeof(ySoP_n)));
            buf.insert(buf.end(),(const char*)&xSoP_n,(const char*)(&xSoP_n+sizeof(xSoP_n)));
            buf.insert(buf.end(),(const char*)&ang_n,(const char*)(&ang_n+sizeof(ang_n)));
        }
        else{
            buf.insert(buf.end(),(const char*)&ySoP,(const char*)(&ySoP+sizeof(ySoP)));
            buf.insert(buf.end(),(const char*)&xSoP,(const char*)(&xSoP+sizeof(xSoP)));
            buf.insert(buf.end(),(const char*)&ang,(const char*)(&ang+sizeof(ang)));
        }
    }
    void deserialize(std::span<const char> buf){
        if(buf.size()!=GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::serial_size())
            return;
        else{
            GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>::deserialize(buf);
            uint16_t offset = GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>::serial_size();
            if(network::utility::is_little_endian()){
                ySoP = ntohf(*(decltype(ySoP)*)(buf.data()+offset));
                offset+=sizeof(ySoP);
                xSoP = ntohf(*(decltype(xSoP)*)(buf.data()+offset));
                offset+=sizeof(xSoP);
                ang = ntohd((*(decltype(ang)*)(buf.data()+offset)));
            }
            else{
                ySoP = *(decltype(ySoP)*)(buf.data()+offset);
                offset+=sizeof(ySoP);
                xSoP = *(decltype(xSoP)*)(buf.data()+offset);
                offset+=sizeof(xSoP);
                ang = *(decltype(ang)*)(buf.data()+offset);
            }
        }
    }

    /// @brief Full buffer size after serialization of GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>
    /// @return uint8_t
    constexpr static uint8_t serial_size(){
        return GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>::serial_size() + sizeof(ySoP)+sizeof(xSoP)+sizeof(ang);
    }
    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return GridDefinition<RepresentationType::GAUSSIAN>::section_size()+10;  //including reserved
    }
};

/** @brief Spherical harmonic grid
    @note Notes: 
    1.  The pentagonal representation of resolution is general.
        Some common truncations are special cases of the pentagonal one: Triangular M = J = K Rhomboidal K = J + M Trapezoidal K = J, K > M
    2.  The representation type (octet 13) indicates the method used to define the norm.
    3.  The representation mode (octet 14) indicates the order of the coefficients, whether global or hemispheric data are depicted,
        and the nature of the parameter stored (symmetric or antisymmetric).
    4.  See Notes (6) to (11) under Grid definition latitude/longitude grid (or equidistant cylindrical, or Plate Carrée) page I.2 Bi 8.

    /// @attention bytes 15-32: reserved (set to zero)
*/
template<>
struct GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>:
        virtual GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>{
    /// @brief bytes 33-35: Latitude of pole of stretching in millidegrees (signed integer) (should multiplied by 0.001)
    float ysp;
    /// @brief bytes 36-38: Longitude of pole of stretching in millidegrees (signed integer) (should multiplied by 0.001)
    float xsp;
    /// @brief bytes 39-42: Stretching factor (representation as for the reference value)
    double s_factor;
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;

    void serialize(std::vector<char>& buf) const{
        GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>::serialize(buf);
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
    void deserialize(std::span<const char> buf){
        if(buf.size()!=GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::serial_size())
            return;
        else{
            GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>::deserialize(buf);
            uint16_t offset = GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>::serial_size();
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
    /// @brief Full buffer size after serialization of GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>
    /// @return uint8_t
    constexpr static uint8_t serial_size(){
        return GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>::serial_size() + sizeof(ysp)+sizeof(xsp)+sizeof(s_factor);
    }
    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>::section_size()+10;  //including reserved
    }
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>:
    GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>,
    GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>{
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(std::span<const char> buf);

    /// @brief Full buffer size after serialization of GridDefinition<RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>
    /// @return uint8_t
    constexpr static uint8_t serial_size(){
        return GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>::serial_size() +
         + sizeof(ySoP)+sizeof(xSoP)+sizeof(ang);
    }
    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>::section_size()+10;  //including reserved
    }
};