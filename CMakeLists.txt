cmake_minimum_required(VERSION 3.15)
project(ECMWF_grib_decoder)

project(ECMWF_grib_decoder VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_C_COMPILER "/usr/bin/gcc")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE GRIB_SOURCES ${CMAKE_CURRENT_LIST_DIR} *.c)

add_library(grib_c STATIC ${GRIB_SOURCES})

add_executable(ECMWF_GD main.cpp functions.cpp)

include_directories(${CMAKE_CURRENT_LIST_DIR}/grib/include)

target_link_libraries(ECMWF_GD PRIVATE grib_c)
target_link_directories(ECMWF_GD PRIVATE ${CMAKE_CURRENT_LIST_DIR}/grib/include
                                        ${CMAKE_CURRENT_LIST_DIR}/grib/src
                                        ${CMAKE_CURRENT_LIST_DIR}/include
                                        ${CMAKE_CURRENT_LIST_DIR}/src)

set_target_properties(ECMWF_GD grib_c
                    PROPERTIES
                    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/lib/Debug"
                    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/shared/Debug"
                    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/bin/Debug"
                    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/Release"
                    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/shared/Release"
                    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/bin/Release")   