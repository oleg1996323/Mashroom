#pragma once
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include <vector>

#ifdef __cplusplus
#include <span>

namespace grid{

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
struct GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>:
        GridDefinitionBase<RepresentationType::ALBERS_EQUAL_AREA,GridModification::NONE>{                             
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;    
};
#endif
}