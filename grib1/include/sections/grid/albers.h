#pragma once
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include <vector>

#ifdef __cplusplus
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

    /// @attention bytes 41-42: reserved
*/
template<>
struct GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>{
    /// @brief bytes 11-13: La1 latitude of first grid point (signed)
    float y1;
    /// @brief bytes 14-16: Lo1 longitude of first grid point (signed)
    float x1;
    /** @brief bytes 18-20: LoV orientation of the grid.
     *  @details The east longitude value of the meridian
     *  which is parallel to the y-axis (or columns
     *  of the grid) along which latitude increases
     *  as the y-coordinate increases (the 
     *  orientation longitude may or may not appear 
     *  on a particular grid) 
    */
    float LoV;
    /// @brief bytes 29-31: Latin 1 first latitude from the pole at which
    /// the secant cone cuts the sphere (unsigned)
    float latin1;
    /// @brief bytes 32-34: Latin 2 second latitude from the pole at which
    /// the secant cone cuts the sphere (unsigned)
    float latin2;
    /// @brief bytes 24-26: Dy y-direction grid length (see Note (2)) (unsigned)
    float Dy;
    /// @brief bytes 21-23: Dx x-direction grid length (see Note (2)) (unsigned)
    float Dx;
    /// @brief bytes 35-37: Latitude of the southern pole in millidegrees (signed integer)
    float latitude_south_pole;      //
    /// @brief bytes 38-40: Longitude of the southern pole in millidegrees (signed integer)
    float longitude_south_pole;     //
    /// @brief bytes 9-10: Ny number of points along y-axis (unsigned)
    uint16_t ny;                    //
    /// @brief bytes 7-8: Nx number of points along x-axis (unsigned)
    uint16_t nx;                    //
    /// @brief byte 27: Projection centre flag (see Note (5))
    ///  bit 1 set to 0 if North Pole is on the projection plane
    ///  bit 1 set to 1 if South Pole is on the projection plane
    bool is_south_pole;
    /// @brief byte 27: Projection centre flag (see Note (5))
    /// bit 2 set to 0 if only one projection centre is used
    /// bit 2 set to 1 if projection is bi-polar and symmetric.              
    bool is_bipolar;
    /// @brief byte 28: Scanning mode (flags see Flag/Code table 8)
    ScanMode scan_mode;
    /// @brief byte 17: Resolution and component flags (see Code table 7)
    ResolutionComponentFlags resolutionAndComponentFlags;
                             
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;

    /// @todo add ErrorCode or bool
    /// @param[in,out] buf
    void serialize(std::vector<char>& buf) const;

    /// @todo add ErrorCode or bool
    /// @param[in] buf
    void deserialize(std::span<const char> buf);
    constexpr static uint8_t serial_size(){
        return sizeof(x1)+sizeof(y1)+sizeof(nx)+sizeof(ny)+sizeof(LoV)+sizeof(Dx)+
                sizeof(Dy)+sizeof(is_south_pole)+sizeof(is_bipolar)+sizeof(latin1)+
                sizeof(latin2)+sizeof(scan_mode)+sizeof(resolutionAndComponentFlags)+
                sizeof(longitude_south_pole)+sizeof(latitude_south_pole);
    }
    constexpr static size_t section_size(){
        return 36; //including reserved
    }
};
#endif