#pragma once
#include <vector>
#include <span>
#include <cinttypes>
#include "ibmtofloat.h"
#include "def.h"
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "grid_additional.h"

namespace grid{

template<RepresentationType REP_T> struct GridDefinition;
template<RepresentationType REP_T> struct GridBase;

template<>
struct GridBase<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>{
    /// @brief La1 latitude of first grid point
    float y1;
    /// @brief Lo1 longitude of first grid point
    float x1;
    /// @brief La2 latitude of last grid point
    float y2;
    /// @brief Lo2 longitude of last grid point
    float x2;
    /// @brief Di i direction increment
    float dy;
    /// @brief Dj j direction increment
    float dx;
    /// @brief Ni number of points along a parallel
    uint16_t ny;
    /// @brief Nj number of points along a meridian
    uint16_t nx;
    /// @brief Scanning mode (flags see Flag/Code table 8)
    ScanMode scan_mode;
    /// @brief Resolution and component flags (see Code table 7)
    ResolutionComponentFlags resolutionAndComponentFlags;

    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return 26;
    }
    constexpr static uint8_t begin_byte(){
        return 6;
    }

    bool operator==(const GridBase& other) const{
        return y1==other.y1 &&
        x1==other.x1 &&
        y2==other.y2 &&
        x2==other.x2 &&
        dy==other.dy &&
        dx==other.dx &&
        ny==other.ny &&
        nx==other.nx &&
        scan_mode==other.scan_mode &&
        resolutionAndComponentFlags==other.resolutionAndComponentFlags;
    }

    GridBase() = default;
    GridBase(unsigned char* buffer):
    nx(GDS_LatLon_nx(buffer)),
    ny(GDS_LatLon_ny(buffer)),
    y1(float(GDS_LatLon_La1(buffer))/1000),
    x1(float(GDS_LatLon_Lo1(buffer))/1000),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_LatLon_mode(buffer))),
    y2(float(GDS_LatLon_La2(buffer))/1000),
    x2(float(GDS_LatLon_Lo2(buffer))/1000),
    dy(float(GDS_LatLon_dy(buffer))/1000),
    dx(float(GDS_LatLon_dx(buffer))/1000),
    scan_mode(ScanMode(GDS_LatLon_scan(buffer))){}
    GridBase(const GridBase&) = default;
    GridBase(GridBase&&) noexcept = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase& operator=(GridBase&&) noexcept = default;
};

template<>
struct GridBase<RepresentationType::GAUSSIAN>{
    /// @brief La1 latitude of first grid point
    float y1 = UNDEFINED;
    /// @brief Lo1 longitude of first grid point
    float x1 = -UNDEFINED;
    /// @brief La2 latitude of last grid point
    float y2 = -UNDEFINED;
    /// @brief Lo2 longitude of last grid point
    float x2 = +UNDEFINED;
    /// @brief Ni number of points along a parallel (unsigned)
    uint16_t ny = 0;
    /// @brief Nj number of points along a meridian (signed)
    int16_t nx = 0;
    /// @brief Di i direction increment
    uint16_t directionIncrement = 0;
    /// @brief N number of parallels between a pole and the equator
    uint16_t N = 0;
    /// @brief Scanning mode (flags see Flag/Code table 8)
    ScanMode scan_mode = {};
    /// @brief Resolution and component flags (see Code table 7)
    ResolutionComponentFlags resolutionAndComponentFlags = {};

    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return 26;
    }
    constexpr static uint8_t begin_byte(){
        return 6;
    }

    bool operator==(const GridBase& other) const{
        return y1==other.y1 &&
        x1==other.x1 &&
        y2==other.y2 &&
        x2==other.x2 &&
        N==other.N &&
        directionIncrement==other.directionIncrement &&
        ny==other.ny &&
        nx==other.nx &&
        scan_mode==other.scan_mode &&
        resolutionAndComponentFlags==other.resolutionAndComponentFlags;
    }
    GridBase() = default;
    GridBase(unsigned char* buffer):
    y1(float(GDS_LatLon_La1(buffer))/1000),
    x1(float(GDS_LatLon_Lo1(buffer))/1000),
    y2(float(GDS_LatLon_La2(buffer))/1000),
    x2(float(GDS_LatLon_Lo2(buffer))/1000),
    nx(GDS_LatLon_nx(buffer)),
    ny(GDS_LatLon_ny(buffer)),
    directionIncrement(GDS_LatLon_dx(buffer)),
    N(GDS_Gaussian_nlat(buffer)),
    scan_mode(ScanMode(GDS_LatLon_scan(buffer))),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_LatLon_mode(buffer))){}
    GridBase(const GridBase&) = default;
    GridBase(GridBase&&) noexcept = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase& operator=(GridBase&&) noexcept = default;
};

template<>
struct GridBase<RepresentationType::ALBERS_EQUAL_AREA>{
/// @brief bytes 11-13: La1 latitude of first grid point (signed)
    float y1 = UNDEFINED;
    /// @brief bytes 14-16: Lo1 longitude of first grid point (signed)
    float x1 = UNDEFINED;
    /** @brief bytes 18-20: LoV orientation of the grid.
     *  @details The east longitude value of the meridian
     *  which is parallel to the y-axis (or columns
     *  of the grid) along which latitude increases
     *  as the y-coordinate increases (the 
     *  orientation longitude may or may not appear 
     *  on a particular grid) 
    */
    float LoV = UNDEFINED;
    /// @brief bytes 29-31: Latin 1 first latitude from the pole at which
    /// the secant cone cuts the sphere (unsigned)
    float latin1 = UNDEFINED;
    /// @brief bytes 32-34: Latin 2 second latitude from the pole at which
    /// the secant cone cuts the sphere (unsigned)
    float latin2 = UNDEFINED;
    /// @brief bytes 24-26: Dy y-direction grid length (see Note (2)) (unsigned)
    float Dy = UNDEFINED;
    /// @brief bytes 21-23: Dx x-direction grid length (see Note (2)) (unsigned)
    float Dx = UNDEFINED;
    /// @brief bytes 35-37: Latitude of the southern pole in millidegrees (signed integer)
    float latitude_south_pole = UNDEFINED;
    /// @brief bytes 38-40: Longitude of the southern pole in millidegrees (signed integer)
    float longitude_south_pole = UNDEFINED;
    /// @brief bytes 9-10: Ny number of points along y-axis (unsigned)
    uint16_t ny = 0;
    /// @brief bytes 7-8: Nx number of points along x-axis (unsigned)
    uint16_t nx = 0;
    /// @brief byte 27: Projection centre flag (see Note (5))
    ///  bit 1 set to 0 if North Pole is on the projection plane
    ///  bit 1 set to 1 if South Pole is on the projection plane
    bool is_south_pole = false;
    /// @brief byte 27: Projection centre flag (see Note (5))
    /// bit 2 set to 0 if only one projection centre is used
    /// bit 2 set to 1 if projection is bi-polar and symmetric.              
    bool is_bipolar = false;
    /// @brief byte 28: Scanning mode (flags see Flag/Code table 8)
    ScanMode scan_mode = {};
    /// @brief byte 17: Resolution and component flags (see Code table 7)
    ResolutionComponentFlags resolutionAndComponentFlags = {};

    constexpr static size_t section_size(){
        return 36; //including reserved
    }

    constexpr static uint8_t begin_byte(){
        return 6;
    }
    bool operator==(const GridBase& other) const{
        return y1==other.y1 &&
        x1==other.x1 &&
        LoV==other.LoV &&
        latin1==other.latin1 &&
        latin2==other.latin2 &&
        Dx==other.Dx &&
        Dy==other.Dy &&
        is_south_pole==other.is_south_pole &&
        is_bipolar==other.is_bipolar &&
        latitude_south_pole==other.latitude_south_pole &&
        longitude_south_pole==other.longitude_south_pole &&
        ny==other.ny &&
        nx==other.nx &&
        scan_mode==other.scan_mode &&
        resolutionAndComponentFlags==other.resolutionAndComponentFlags;
    }
    GridBase() = default;
    GridBase(unsigned char* buffer):
    nx(GDS_Lambert_nx(buffer)),
    ny(GDS_Lambert_ny(buffer)),
    y1(float(GDS_Lambert_La1(buffer))/1000),
    x1(float(GDS_Lambert_Lo1(buffer))/1000),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_Lambert_mode(buffer))),
    LoV(float(GDS_Lambert_Lov(buffer))/1000),
    Dy(float(GDS_Lambert_dy(buffer))/1000),
    Dx(float(GDS_Lambert_dx(buffer))/1000),
    is_south_pole(!GDS_Lambert_NP(buffer)),
    is_bipolar(GDS_Lambert_BP(buffer)),
    scan_mode(ScanMode(GDS_Lambert_scan(buffer))),
    latin1(float(GDS_Lambert_Latin1(buffer))/1000),
    latin2(float(GDS_Lambert_Latin2(buffer))/1000),
    latitude_south_pole(float(GDS_Lambert_LatSP(buffer))/1000),
    longitude_south_pole(float(GDS_Lambert_LonSP(buffer))/1000){}
    GridBase(const GridBase&) = default;
    GridBase(GridBase&&) noexcept = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase& operator=(GridBase&&) noexcept = default;
};

#include "code_tables/table_10.h"
template<>
struct GridBase<SPHERICAL_HARMONIC_COEFFICIENTS>{
    /// @brief bytes 7-8: J pentagonal resolution parameter (unsigned)
    uint16_t J = 0;
    /// @brief bytes 9-10: K pentagonal resolution parameter (unsigned)
    uint16_t K = 0; 
    /// @brief bytes 11-12: M pentagonal resolution parameter (unsigned)
    uint16_t M = 0;
    /**
     * @brief byte 13: Representation type (see Code table 9)
     * @details Only accessible for ECMWF:
     * Associated Legendre Polynomials of the
     * First Kind with normalization such that
     * the integral equals 1
    */
    bool representation_type = false;
    /**
     * @brief byte 14: Representation mode (see Code table 10)
     * @details Accessible modes for ECMWF:
     * @addindex 
     */
    Spectral rep_mode = {};
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
    constexpr static uint8_t begin_byte(){
        return 6;
    }
    bool operator==(const GridBase& other) const{
        return K==other.K &&
        M==other.M &&
        J==other.J &&
        rep_mode == other.rep_mode &&
        representation_type == other.representation_type;
    }
    GridBase() = default;
    GridBase(unsigned char* buffer):
        J(GDS_Harmonic_nj(buffer)),
        K(GDS_Harmonic_nk(buffer)),
        M(GDS_Harmonic_nm(buffer)),
        representation_type(GDS_Harmonic_type(buffer)),
        rep_mode((Spectral)GDS_Harmonic_mode(buffer)){}
    GridBase(const GridBase&) = default;
    GridBase(GridBase&&) noexcept = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase& operator=(GridBase&&) noexcept = default;
};

template<>
struct GridBase<LAMBERT>{
    /// @brief bytes 7-8: Nx number of points along x-axis (unsigned)
    uint16_t nx;
    /// @brief bytes 9-10: Ny number of points along y-axis (unsigned)
    uint16_t ny;
    /// @brief bytes 11-13: La1 latitude of first grid point
    float y1;
    /// @brief bytes 14-16: Lo1 longitude of first grid point
    float x1;
    /// @brief byte 17: Resolution and component flags (see Code table 7)
    ResolutionComponentFlags resolutionAndComponentFlags;
    /// @brief bytes 18-20: LoV orientation of the grid;
    /// i.e. the east longitude value of
    /// the meridian which is parallel to
    /// the y-axis (or columns of the grid)
    /// along which latitude increases as
    /// the y-coordinate increases (the orientation
    /// longitude may or may not appear on
    /// a particular grid)
    float LoV;
    /// @brief bytes 21-23: Dx x-direction grid length (see Note (2))
    float dx;
    /// @brief bytes 24-26: Dy y-direction grid length (see Note (2))
    float dy;
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
    /// @brief bytes 29-31: Latin 1 first latitude from
    /// the pole at which the secant cone cuts the sphere
    float latin1;
    /// @brief bytes 32-34: Latin 2 second latitude from
    /// the pole at which the secant cone cuts the sphere (signed)
    float latin2;
    /// @brief bytes 35-37: Latitude of the southern pole in millidegrees (integer)
    int latitude_south_pole;
    /// @brief bytes 38-40: Longitude of the southern pole in millidegrees (integer)
    int longitude_south_pole;

    /// @brief Full buffer size after serialization of GridDefinition<RepresentationType::LAMBERT>
    /// @return uint8_t
    constexpr static uint8_t serial_size(){
        return sizeof(nx)+sizeof(ny)+sizeof(x1)+sizeof(y1)+sizeof(resolutionAndComponentFlags)+
        sizeof(LoV)+sizeof(dx)+sizeof(dy)+sizeof(is_bipolar)+sizeof(is_south_pole)+sizeof(scan_mode)+
        sizeof(latin1)+sizeof(latin2)+sizeof(latitude_south_pole)+sizeof(longitude_south_pole);
    }
    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return 36;
    }
    constexpr static uint8_t begin_byte(){
        return 6;
    }

    bool operator==(const GridBase& other) const{
        return y1==other.y1 &&
        x1==other.x1 &&
        LoV==other.LoV &&
        latin1==other.latin1 &&
        latin2==other.latin2 &&
        dx==other.dx &&
        dy==other.dy &&
        is_south_pole==other.is_south_pole &&
        is_bipolar==other.is_bipolar &&
        latitude_south_pole==other.latitude_south_pole &&
        longitude_south_pole==other.longitude_south_pole &&
        ny==other.ny &&
        nx==other.nx &&
        scan_mode==other.scan_mode &&
        resolutionAndComponentFlags==other.resolutionAndComponentFlags;
    }

    GridBase() = default;
    GridBase(unsigned char* buffer):
        nx(GDS_Lambert_nx(buffer)),
        ny(GDS_Lambert_ny(buffer)),
        y1(float(GDS_Lambert_La1(buffer))/1000),
        x1(float(GDS_Lambert_Lo1(buffer))/1000),
        resolutionAndComponentFlags(ResolutionComponentFlags(GDS_Lambert_mode(buffer))),
        LoV(GDS_Lambert_Lov(buffer)),
        dx(float(GDS_Lambert_dx(buffer))/1000),
        dy(float(GDS_Lambert_dy(buffer))/1000),
        is_south_pole(!GDS_Lambert_NP(buffer)), //GDS_Lambert_NP
        is_bipolar(GDS_Lambert_BP(buffer)),
        scan_mode(ScanMode(GDS_Lambert_scan(buffer))),
        latin1(GDS_Lambert_Latin1(buffer)),
        latin2(GDS_Lambert_Latin2(buffer)),
        latitude_south_pole(GDS_Lambert_LatSP(buffer)),
        longitude_south_pole(GDS_Lambert_LonSP(buffer)){}
    GridBase(const GridBase&) = default;
    GridBase(GridBase&&) noexcept = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase& operator=(GridBase&&) noexcept = default;
};

template<>
struct GridBase<POLAR_STEREOGRAPH_PROJ>{
    /// @brief bytes 7-8: Nx number of points along x-axis (unsigned)
    uint16_t nx;
    /// @brief bytes 9-10: Ny number of points along y-axis (unsigned)
    uint16_t ny;
    /// @brief bytes 11-13: La1 latitude of first grid point (signed) (millidegrees)
    float y1;
    /// @brief bytes 14-16: Lo1 longitude of first grid point (signed) (millidegrees)
    float x1;
    /// @brief byte 17: Resolution and component flags (see Code table 7)
    ResolutionComponentFlags resolutionAndComponentFlags;
    /// @brief bytes 18-20: LoV orientation of the grid;
    /// i.e. the longitude value of the meridian
    /// which is parallel to the y-axis
    /// (or columns of the grid) along which latitude
    /// increases as the Y-coordinate increases
    /// (the orientation longitude may or may not appear on a particular grid)
    float LoV;
    /// @brief bytes 21-23: Dx X-direction grid length (see Note (2)) (unsigned)
    uint32_t dx;
    /// @brief bytes 24-26: Dy Y-direction grid length (see Note (2)) (unsigned)
    uint32_t dy;
    /// @brief byte 27: Projection centre flag (see Note (5))
    /// Octet 27 (projection centre flag):
    /// Latitude and longitude are in millidegrees (thousandths of a degree).
    /// bit 1 set to 1 if South Pole is on the projection plane
    /// (to be set up as flag table).
    bool is_south_pole;
    /// @brief byte 28: Scanning mode (flags see Flag/Code table 8)
    ScanMode scan_mode;

    /// @brief Full buffer size after serialization of GridDefinition<RepresentationType::POLAR_STEREOGRAPH_PROJ>
    /// @return uint8_t
    constexpr static uint8_t serial_size(){
        return sizeof(nx)+sizeof(ny)+sizeof(x1)+sizeof(y1)+sizeof(resolutionAndComponentFlags)+
        sizeof(LoV)+sizeof(dx)+sizeof(dy)+sizeof(is_south_pole)+sizeof(scan_mode);
    }
    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return 26;
    }
    constexpr static uint8_t begin_byte(){
        return 6;
    }

    bool operator==(const GridBase& other) const{
        return y1==other.y1 &&
        x1==other.x1 &&
        LoV==other.LoV &&
        dx==other.dx &&
        dy==other.dy &&
        is_south_pole==other.is_south_pole &&
        ny==other.ny &&
        nx==other.nx &&
        scan_mode==other.scan_mode &&
        resolutionAndComponentFlags==other.resolutionAndComponentFlags;
    }

    GridBase() = default;
    GridBase(unsigned char* buffer):
    nx(GDS_Polar_nx(buffer)),
    ny(GDS_Polar_ny(buffer)),
    y1(float(GDS_Polar_La1(buffer))/1000),
    x1(float(GDS_Polar_Lo1(buffer))/1000),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_Polar_mode(buffer))),
    LoV(float(GDS_Polar_LoV(buffer))/1000),
    dx(GDS_Polar_Dx(buffer)),
    dy(GDS_Polar_Dy(buffer)),
    is_south_pole(GDS_Polar_pole(buffer)),
    scan_mode(ScanMode(GDS_Polar_mode(buffer))){}
    GridBase(const GridBase&) = default;
    GridBase(GridBase&&) noexcept = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase& operator=(GridBase&&) noexcept = default;
};

/// @ref https://web.unbc.ca/~funnel0/fst2grib/resources/GRIB.html#data_rep
template<>
struct GridBase<SPACE_VIEW>{
    /// @brief bytes 7-8: Nx number of points along x-axis (unsigned)
    uint16_t nx;
    /// @brief bytes 9-10: Ny number of points along y-axis (unsigned)
    uint16_t ny;
    /// @brief bytes 11-13: Lap - latitude of sub-satellite point (signed) (millidegrees)
    float y;
    /// @brief bytes 14-16: Lop - longitude of sub-satellite point (signed) (millidegrees)
    float x;
    /// @brief byte 17: Resolution and component flags (see Code table 7)
    ResolutionComponentFlags resolutionAndComponentFlags;
    /// @brief bytes 18-20: dx - apparent diameter of earth in grid lengths, in x direction (unsigned)
    uint32_t dx;
    /// @brief bytes 21-23: dy - apparent diameter of earth in grid lengths, in y direction (unsigned)
    uint32_t dy;
    /// @brief bytes 24-25: Xp - X-coordinate of sub satellite point
    int16_t xp;
    /// @brief bytes 26-27: Yp - Y-coordinate of sub-satellite point
    int16_t yp;
    /// @brief byte 28: Scanning mode (flags see Flag/Code table 8)
    ScanMode scan_mode;
    /// @brief bytes 29-31: the orientation of the grid;
    /// i.e., the angle in millidegrees between the increasing y
    /// axis and the meridian of the sub-satellite point in the
    /// direction of increasing latitude (see Note 3).
    uint32_t orientation_;
    /// @brief Nr - the altitude of the camera from the earth's center,
    /// measured in units of the earth's (equatorial) radius (See Note 4).
    uint32_t nr;
    /// @brief bytes 35-36: Xo – x-coordinate of origin of sector image
    int16_t Xo;
    /// @brief bytes 37-38: Yo – y-coordinate of origin of sector image
    int16_t Yo;

    /// @brief Full buffer size after serialization of GridDefinition<RepresentationType::POLAR_STEREOGRAPH_PROJ>
    /// @return uint8_t
    constexpr static uint8_t serial_size(){
        return sizeof(nx)+sizeof(ny)+sizeof(y)+sizeof(x)+sizeof(resolutionAndComponentFlags)+
        sizeof(dx)+sizeof(dy)+sizeof(xp)+sizeof(yp)+sizeof(scan_mode)+
        sizeof(orientation_)+sizeof(nr)+sizeof(Xo)+sizeof(Yo);
    }
    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return 38;
    }
    constexpr static uint8_t begin_byte(){
        return 6;
    }

    bool operator==(const GridBase& other) const{
        return y==other.y &&
        x==other.x &&
        xp==other.xp &&
        yp==other.yp &&
        dx==other.dx &&
        dy==other.dy &&
        orientation_==other.orientation_ &&
        nr==other.nr &&
        Xo==other.Xo &&
        Yo==other.Yo &&
        ny==other.ny &&
        nx==other.nx &&
        scan_mode==other.scan_mode &&
        resolutionAndComponentFlags==other.resolutionAndComponentFlags;
    }

    GridBase() = default;
    GridBase(unsigned char* buffer):
    nx(GDS_Polar_nx(buffer)),
    ny(GDS_Polar_ny(buffer)),
    y(float(GDS_Polar_La1(buffer))/1000),
    x(float(GDS_Polar_Lo1(buffer))/1000),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_Polar_mode(buffer))),
    dx(read_bytes<3,false>(&buffer[17])),
    dy(read_bytes<3,false>(&buffer[20])),
    xp(read_bytes<2,true>(&buffer[23])),
    yp(read_bytes<2,true>(&buffer[25])),
    scan_mode((ScanMode)buffer[27]),
    orientation_(read_bytes<3,false>(&buffer[28])),
    nr(read_bytes<3,false>(&buffer[31])),
    Xo(read_bytes<2,true>(&buffer[34])),
    Yo(read_bytes<2,true>(&buffer[36])){}
    GridBase(const GridBase&) = default;
    GridBase(GridBase&&) noexcept = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase& operator=(GridBase&&) noexcept = default;
};

template<>
struct GridBase<MERCATOR>{
    /// @brief bytes 7-8: Ni number of points along a parallel (signed)
    int16_t nx;
    /// @brief bytes 9-10: Nj number of points along a meridian
    int16_t ny;
    /// @brief bytes 11-13: La1 latitude of first grid point
    float y1;
    /// @brief bytes 14-16: Lo1 longitude of first grid point
    float x1;
    /// @brief byte 17: Resolution and component flags (see Code table 7)
    ResolutionComponentFlags resolutionAndComponentFlags;
    /// @brief bytes 18-20: La2 latitude of last grid point
    float y2;
    /// @brief bytes 21-23: Lo2 longitude of last grid point
    float x2;
    /// @brief bytes 24-26: Latin latitude(s) at which
    /// the Mercator projection cylinder intersects the Earth
    float latin;
    /// @brief byte 28: Scanning mode (flags see Flag/Code table 8)
    ScanMode scan_mode;
    /// @brief bytes 29-31: Di longitudinal direction grid length (see Note (2))
    float dy;
    /// @brief bytes 32-34: Dj latitudinal direction grid length (see Note (2))
    float dx;
    
    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return 36;
    }
    constexpr static uint8_t begin_byte(){
        return 6;
    }

    bool operator==(const GridBase& other) const{
        return y1==other.y1 &&
        x1==other.x1 &&
        latin==other.latin &&
        dx==other.dx &&
        dy==other.dy &&
        x2==other.x2 &&
        y2==other.y2 &&
        ny==other.ny &&
        nx==other.nx &&
        scan_mode==other.scan_mode &&
        resolutionAndComponentFlags==other.resolutionAndComponentFlags;
    }

    GridBase() = default;
    GridBase(unsigned char* buffer):
    nx(GDS_Merc_nx(buffer)),
    ny(GDS_Merc_dy(buffer)),
    y1(GDS_Merc_La1(buffer)),
    x1(GDS_Merc_Lo1(buffer)),
    resolutionAndComponentFlags(GDS_Merc_mode(buffer)),
    y2(GDS_Merc_La2(buffer)),
    x2(GDS_Merc_Lo2(buffer)),
    latin(GDS_Merc_Latin(buffer)),
    scan_mode(GDS_Merc_scan(buffer)),
    dy(GDS_Merc_dy(buffer)),
    dx(GDS_Merc_dx(buffer)){}
    GridBase(const GridBase&) = default;
    GridBase(GridBase&&) noexcept = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase& operator=(GridBase&&) noexcept = default;
};

template<>
struct GridBase<GNOMONIC>{
    constexpr static size_t section_size(){
        return 0;
    }
    constexpr static uint8_t begin_byte(){
        return 6;
    }

    bool operator==(const GridBase& other) const{
        return true;
    }

    GridBase() = default;
    GridBase(unsigned char* buffer){}
    GridBase(const GridBase&) = default;
    GridBase(GridBase&&) noexcept = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase& operator=(GridBase&&) noexcept = default;
};

template<>
struct GridBase<MILLERS_CYLINDR>{
    constexpr static size_t section_size(){
        return 0;
    }
    constexpr static uint8_t begin_byte(){
        return 6;
    }

    bool operator==(const GridBase& other) const{
        return true;
    }

    GridBase() = default;
    GridBase(unsigned char* buffer){}
    GridBase(const GridBase&) = default;
    GridBase(GridBase&&) noexcept = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase& operator=(GridBase&&) noexcept = default;
};

template<>
struct GridBase<SIMPLE_POLYCONIC>{
    constexpr static size_t section_size(){
        return 0;
    }
    constexpr static uint8_t begin_byte(){
        return 6;
    }

    bool operator==(const GridBase& other) const{
        return true;
    }

    GridBase() = default;
    GridBase(unsigned char* buffer){}
    GridBase(const GridBase&) = default;
    GridBase(GridBase&&) noexcept = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase& operator=(GridBase&&) noexcept = default;
};

template<>
struct GridBase<RepresentationType::UTM>{
    constexpr static size_t section_size(){
        return 0;
    }
    constexpr static uint8_t begin_byte(){
        return 6;
    }

    bool operator==(const GridBase& other) const{
        return true;
    }

    GridBase() = default;
    GridBase(unsigned char* buffer){}
    GridBase(const GridBase&) = default;
    GridBase(GridBase&&) noexcept = default;
    GridBase& operator=(const GridBase&) = default;
    GridBase& operator=(GridBase&&) noexcept = default;
};

template<RepresentationType REP_T,GridModification MOD>
struct GridDefinitionBase{
    constexpr static RepresentationType type(){
        return REP_T;
    }
    constexpr static GridModification modification(){
        return MOD;
    }
	GridBase<REP_T> base_;
	GridAdditional<REP_T,MOD> additional_;
	bool operator==(const GridDefinitionBase<REP_T,MOD>& other) const{
		return base_==other.base_ && additional_==other.additional_;
	}

    /// @brief Full size of grid information section (including reserved bytes)
    /// @return unsigned long
    /// @attention Including reserved bytes
    constexpr static size_t section_size(){
        return decltype(base_)::section_size() + decltype(additional_)::section_size();
    }
    constexpr static uint8_t begin_byte(){
        return decltype(base_)::begin_byte();
    }
    protected:
    GridDefinitionBase(){
        GridBase<REP_T> b;
        GridAdditional<REP_T,MOD> a;
    }
    GridDefinitionBase(unsigned char* buffer):
        base_(buffer),additional_(buffer){}
    GridDefinitionBase(const GridDefinitionBase&) = default;
    GridDefinitionBase(GridDefinitionBase&&) noexcept = default;
    GridDefinitionBase& operator=(const GridDefinitionBase&) = default;
    GridDefinitionBase& operator=(GridDefinitionBase&&) noexcept = default;
};

template<RepresentationType REP_T>
constexpr size_t GridAdditional<REP_T,GridModification::NONE>::begin_byte(){
    return 	GridBase<REP_T>::begin_byte()+
            GridBase<REP_T>::section_size();
}

template<RepresentationType REP_T>
constexpr size_t GridAdditional<REP_T,GridModification::ROTATION>::begin_byte(){
    return 	GridBase<REP_T>::begin_byte()+
            GridBase<REP_T>::section_size();
}

template<RepresentationType REP_T>
constexpr size_t GridAdditional<REP_T,GridModification::STRETCHING>::begin_byte(){
    return 	GridBase<REP_T>::begin_byte()+
            GridBase<REP_T>::section_size();
}
template<RepresentationType REP_T>
constexpr size_t GridAdditional<REP_T,GridModification::ROTATION_STRETCHING>::begin_byte(){
    return 	GridBase<REP_T>::begin_byte()+
            GridBase<REP_T>::section_size();
}
}


namespace serialization{
    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Serialize<NETWORK_ORDER,grid::GridDefinition<REP>>{
        using type = grid::GridDefinition<REP>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.base_,msg.additional_);
        }
    };

    template<bool NETWORK_ORDER,RepresentationType REP>
    struct Deserialize<NETWORK_ORDER,grid::GridDefinition<REP>>{
        using type = grid::GridDefinition<REP>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.base_,msg.additional_);
        }
    };

    template<RepresentationType REP>
    struct Serial_size<grid::GridDefinition<REP>>{
        using type = grid::GridDefinition<REP>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.base_,msg.additional_);
        }
    };

    template<RepresentationType REP>
    struct Min_serial_size<grid::GridDefinition<REP>>{
        using type = grid::GridDefinition<REP>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::base_),decltype(type::additional_)>();
        }();
    };

    template<RepresentationType REP>
    struct Max_serial_size<grid::GridDefinition<REP>>{
        using type = grid::GridDefinition<REP>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::base_),decltype(type::additional_)>();
        }();
    };
}