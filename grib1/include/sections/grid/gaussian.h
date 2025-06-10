#pragma once
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include <span>

/** @brief Simple gaussian grid
    @note Notes: 
    1.  Latitude, longitude and increments are in millidegrees.
    2.  Latitude values are limited to the range 0-90 000; bit 1 is set to 1 to indicate south latitude.
    3.  Longitude values are limited to the range 0-360 000; bit 1 is set to 1 to indicate west longitude.
    4.  The number of parallels between a pole and the equator is used to establish the variable (Gaussian)
        spacing of the parallels; this value must always be given.
    5.  The latitude and longitude of the last grid point and the first grid point should always be given for regular grids.
    6.  Where items are not given, the appropriate octet(s) should have all bits set to 1.
    7.  See Notes (6) to (11) under Grid definition latitude/longitude grid (or equidistant cylindrical, or Plate Carrée) page I.2 Bi 8.
    8.  Quasi-regular Gaussian latitude/longitude grids are defined only for subsets of global grids containing full latitude rows (360°).
    9.  For data on a quasi-regular grid, in which all the rows do not necessarily have the same number of grid points, Ni (octets 7-8)
        and the corresponding Di (octets 24-25) shall be coded with all bits set to 1 (missing); the actual number of points along each
        parallel shall be coded.
    10. A quasi-regular Gaussian latitude/longitude grid is only defined for the grid scanning mode with consecutive points on parallels
        (bit 3 set to zero in Code table 8). The first point in each row shall be positioned at the meridian indicated by octets 14-16
        and the last shall be positioned at the meridian indicated by octets 21-23. The grid points along each parallel shall be evenly
        spaced in longitude.

    /// @attention bytes 29-32: reserved
*/
template<>
struct GridDefinition<RepresentationType::GAUSSIAN>{
    /// @brief La1 latitude of first grid point
    float y1;
    /// @brief Lo1 longitude of first grid point
    float x1;
    /// @brief La2 latitude of last grid point
    float y2;
    /// @brief Lo2 longitude of last grid point
    float x2;
    /// @brief Ni number of points along a parallel (unsigned)
    uint16_t ny;
    /// @brief Nj number of points along a meridian (signed)
    int16_t nx;
    /// @brief Di i direction increment
    uint16_t directionIncrement;
    /// @brief N number of parallels between a pole and the equator
    uint16_t N;
    /// @brief Scanning mode (flags see Flag/Code table 8)
    ScanMode scan_mode;
    /// @brief Resolution and component flags (see Code table 7)
    ResolutionComponentFlags resolutionAndComponentFlags;

    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::GAUSSIAN>& other) const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(std::span<const char> buf);

    /// @brief Full buffer size after serialization of GridDefinition<RepresentationType::GAUSSIAN>
    /// @return uint8_t
    constexpr static uint8_t serial_size(){
        return  sizeof(y1)+sizeof(x1)+sizeof(y2)+sizeof(x2)+
                sizeof(ny)+sizeof(nx)+sizeof(directionIncrement)+
                sizeof(N)+sizeof(scan_mode)+sizeof(resolutionAndComponentFlags);
    }
    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return 26;  //including reserved
    }
};
template<>
struct GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>:virtual GridDefinition<RepresentationType::GAUSSIAN>{
    /// @brief Latitude of the southern pole in millidegrees (integer)
    float yp;
    /// @brief Longitude of the southern pole in millidegrees (integer)
    float xp;
    /// @brief Angle of rotation (represented in the same way as the reference value) (ibmfloat)
    double ang;

    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>& other) const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(std::span<const char> buf);

    constexpr static uint8_t serial_size(){
        return  GridDefinition<RepresentationType::GAUSSIAN>::serial_size() + sizeof(yp)+sizeof(xp)+sizeof(ang);
    }

    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return GridDefinition<RepresentationType::GAUSSIAN>::section_size()+10;  //including reserved
    }
};
template<>
struct GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>:virtual GridDefinition<RepresentationType::GAUSSIAN>{
    float ysp;        //Latitude of pole of stretching in millidegrees (integer)
    float xsp;        //Longitude of pole of stretching in millidegrees (integer)
    double s_factor;    //Stretching factor (representation as for the reference value) (ibmfloat)

    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(std::span<const char> buf);
    bool operator==(const GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>& other) const;

    constexpr static uint8_t serial_size(){
        return GridDefinition<RepresentationType::GAUSSIAN>::serial_size() + sizeof(ysp)+sizeof(xsp)+sizeof(s_factor);
    }

    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return GridDefinition<RepresentationType::GAUSSIAN>::section_size()+10;  //including reserved
    }
};
template<>
struct GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON> final:
    GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>,GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>{

    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(std::span<const char> buf);
    bool operator==(const GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>& other) const;
    constexpr static uint8_t serial_size(){
        return  GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::serial_size() + sizeof(yp)+sizeof(xp)+sizeof(ang);
    }
    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::section_size()+10;  //including reserved
    }
};