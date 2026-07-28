cmake_policy(SET CMP0099 NEW)
cmake_minimum_required(VERSION 3.22)
project("Geoinformational format's API collection")
include(print_all_props.cmake)
option(GRIB1API_ON "compile and include Grib v1 API" ON)
option(GRIB2API_ON "compile and include Grib v2 API" OFF)
option(GRIB3API_ON "compile and include Grib v3 API" OFF)
option(NETCDFAPI_ON "compile and include NetCDF API" OFF)
option(TESTS_ON "compile tests" ON)

set(API_EXTERNALS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
add_library(API_lib INTERFACE)

if(${GRIB1API_ON} OR ${GRIB2API_ON} OR ${GRIB3API_ON} OR ${NETCDFAPI_ON})
    add_subdirectory(common)
    target_link_libraries(API_lib INTERFACE common)
    get_target_property(API_COMMON_INCLUDES common INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(API_lib INTERFACE
    ${API_COMMON_INCLUDES}
    )
    if(${GRIB1API_ON})
        target_compile_definitions(common PUBLIC GRIB1API)
    endif()
    if(${GRIB2API_ON})
        target_compile_definitions(common PUBLIC GRIB2API)
    endif()
    if(${GRIB3API_ON})
        target_compile_definitions(common PUBLIC GRIB3API)
    endif()
    if(${NETCDF_ON})
        target_compile_definitions(common PUBLIC NETCDFAPI)
    endif()
else()
    message(FATAL "no API selected")
endif()

if(${GRIB1API_ON})
    add_subdirectory(grib1)
    target_link_libraries(API_lib INTERFACE grib1API)
    get_target_property(GRIB1_INCLUDES grib1API INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(API_lib INTERFACE
    ${GRIB1_INCLUDES}
    )
endif()
if(${GRIB2API_ON})
    add_subdirectory(grib2)
    target_link_libraries(API_lib INTERFACE grib2API)
    get_target_property(GRIB2_INCLUDES grib2API INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(API_lib INTERFACE
    ${GRIB2_INCLUDES}
    )
endif()
if(${GRIB3API_ON})
    add_subdirectory(grib3)
    target_link_libraries(API_lib INTERFACE grib3API)
    get_target_property(GRIB3_INCLUDES grib3API INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(API_lib INTERFACE
    ${GRIB3_INCLUDES}
    )
endif()
if(${NETCDFAPI_ON})
    add_subdirectory(netcdf)
    target_link_libraries(API_lib INTERFACE netcdfAPI)
    get_target_property(NETCDF1_INCLUDES netcdf1API INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(API_lib INTERFACE
    ${NETCDF1_INCLUDES}
    )
endif()
