cmake_minimum_required(VERSION 3.16)
project("GRIB v1 API" VERSION 0.0.1)
file(GLOB_RECURSE GRIB1_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp)



# target_include_directories(serial INTERFACE include)
# add_library(grib1_headers INTERFACE)
# add_library(gribAPI::v1::headers ALIAS grib1_headers)
# target_sources(gribAPI::v1::headers PUBLIC 
#                 include/properties.h
#                 include/paramtablev.h
#                 include/.h
#                 include/concepts.h)

add_library(grib1API SHARED ${GRIB1_SOURCES})
add_library(gribAPI::v1 ALIAS grib1API)
target_include_directories(grib1API PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
target_link_libraries(grib1API PRIVATE library)
get_target_property(grib1API_INCLUDES grib1API INCLUDE_DIRECTORIES)
message(STATUS "Grib1 include directory: ${grib1API_INCLUDES}")
set(grib1API_INCLUDES ${grib1API_INCLUDES} PARENT_SCOPE)