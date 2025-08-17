#pragma once
#include "product/local.h"
//https://codes.ecmwf.int/grib/format/grib1/local/4/
#ifdef __cplusplus
template<>
struct PDSLocal<ECMWF>::Definition<4>{
    //Ensemble member number
    //50-51, if stream = 1090; 50, otherwise 51, unless stream = 1090
    static unsigned short perturb_number(const PDSLocal<ECMWF>& pds) noexcept{
        if(pds.get_stream()==1090)
            return read_bytes(pds.buf_[49],pds.buf_[50]);
        else return pds.buf_[50];
    }
    //1 (a flag showing a post-auxiliary array is in use)
    static bool flag_showing_post_aux_arr_in_use(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[51];
    }
    //System number
    static unsigned char system_number(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[52];
    }
    //Method number
    static unsigned char method_number(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[53];
    }
    //Space unit flag (applies only if lengths not otherwise labelled):
    //0 = metres i<128 = 10**i metres i>128 = 10**(i-256) metres
    static unsigned char space_unit_flag(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[54];
    }
    //Vertical coordinate definition (z):
    //0 = z above origin
    //1 = R from origin
    //2 = h above mean sea level geopotential
    //3 = h above ground surface
    //160 = Geopotential depth below mean sea level (mm)
    //161 = Ocean isopycnic surface (potential density)
    static unsigned char vertical_coord_definition(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[55];
    }
    //Horizontal coordinate definition (x,y):
    //0 = Latitude/longitude (microdegrees)
    //1 = Cartesian (fundamental origin)
    //2 = Cartesian (shifted, rotated origin) (+info)
    //3 = Regular gaussian grid (microdegrees) (+info)
    //4 = Polar stereographic (+info)
    //5 = Spherical harmonic coefficients (INTEGERs) (+info)
    static unsigned char horizontal_coord_definition(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[56];
    }
    //Time unit flag:
    //0 = Seconds
    //1 = Minutes
    //2 = Hours
    //3 = Days
    //4 = Years
    //5128 = 10**(i-256) seconds
    static unsigned char time_unit_flag(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[57];
    }
    //Time coordinate definition (t):
    //0 = Real Earth time (UTC)
    //1 = Ideal Earth time (360 * 86400s days per year)
    static unsigned char time_coord_definition(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[58];
    }
    //Mixed coordinate field flag:
    //0 = No mixed coordinates
    //1 = x,y coordinates mixed (+info)
    //2 = x,z coordinates mixed (+info)
    //3 = y,z coordinates mixed (+info)
    //4 = x,t coordinates mixed (+info)
    //5 = y,t coordinates mixed (+info)
    //6 = z,t coordinates mixed (+info)
    static unsigned char mixed_coord_field_flag(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[59];
    }
    //Coordinate 1 flag (usually time).
    static unsigned char coord1_flag(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[60];
    }
    //Averaging flag:
    //0 = no averaging (data on/at level 1)
    //1 = inclusive average between level 1 and 2
    //2 = exclusive average between level 1 and 2
    static unsigned char ave1_flag(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[61];
    }
    //Position of level 1
    static int coord1_start(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[62],pds.buf_[63],pds.buf_[64],pds.buf_[65]);
    }
    //Position of level 2 (or zero if not used)
    static int coord1_end(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[66],pds.buf_[67],pds.buf_[68],pds.buf_[69]);
    }
    //Coordinate 2 flag (usually z-coordinate).
    static unsigned char coord2_flag(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[70];
    }
    //Averaging flag:
    //0 = no averaging (data on/at level 1)
    //1 = inclusive average between level 1 and 2
    //2 = exclusive average between level 1 and 2
    static unsigned char ave2_flag(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[71];
    }
    //Position of level 1
    static int coord2_start(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[72],pds.buf_[73],pds.buf_[74],pds.buf_[75]);
    }
    //Position of level 2 (or zero if not used)
    static int coord2_end(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[76],pds.buf_[77],pds.buf_[78],pds.buf_[79]);
    }
    //Coordinate 3 flag (x-axis, usually longitude).
    static unsigned char coord3_flag(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[80];
    }
    //Coordinate 4 flag (y-axis, usually latitude).
    static unsigned char coord4_flag(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[81];
    }
    //Coordinate 4 of first grid point
    static int coord4_first_grid_point(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[82],pds.buf_[83],pds.buf_[84],pds.buf_[85]);
    }
    //Coordinate 3 of first grid point
    static int coord3_first_grid_point(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[86],pds.buf_[87],pds.buf_[88],pds.buf_[89]);
    }
    //Coordinate 4 of last grid point
    static int coord4_last_grid_point(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[90],pds.buf_[91],pds.buf_[92],pds.buf_[93]);
    }
    //Coordinate 3 of last grid point
    static int coord3_last_grid_point(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[94],pds.buf_[95],pds.buf_[96],pds.buf_[97]);
    }
    //i-increment
    static int i_increment(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[98],pds.buf_[99],pds.buf_[100],pds.buf_[101]);
    }
    //j-increment
    static int j_increment(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[102],pds.buf_[103],pds.buf_[104],pds.buf_[105]);
    }
    //Flag for irregular grid coordinate list:
    //0 = none
    //1 = x-axis values (typically longitude)
    //2 = y-axis values (typically latitude)
    //3 = first x-axis, then y-axis values given
    static unsigned char irreg_grid_coord_list_flag(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[106];
    }
    //Flag for normal or staggered grid:
    //0 = normal grid (all rows have same x-coordinate system)
    //1 = staggered grid (odd and even rows have different x-coordinate systems, eg Arakawa grid).
    static unsigned char normal_or_staggered_grid_flag(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[107];
    }
    //Flag for any further information:
    //0 = none
    //1 = auxiliary array contains x-axis topographic depths/heights.
    static unsigned char any_further_information(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[108];
    }
    //Number of entries in the horizontal coordinate definition
    static unsigned char number_horiz_coords(const PDSLocal<ECMWF>& pds) noexcept{
        return pds.buf_[109];
    }
    //Number of entries in mixed coordinate definition
    static unsigned short number_in_mixed_coord_def(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[110],pds.buf_[111]);
    }
    //Number of entries in the grid coordinate list
    static unsigned short number_in_grid_coord_list(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[112],pds.buf_[113]);
    }
    //Number of entries in the auxiliary array
    static unsigned short number_in_aux_array(const PDSLocal<ECMWF>& pds) noexcept{
        return read_bytes(pds.buf_[114],pds.buf_[115]);
    }
    //TODO: add:
    // horizontalCoordinateSupplement
    // mixedCoordinateDefinition
    // gridCoordinate
    // auxiliary
    // postAuxiliary
};
#endif