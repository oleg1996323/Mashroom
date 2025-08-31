cmake_minimum_required(VERSION 3.16)
project("GRIB v1 API" VERSION 0.0.1)
file(GLOB_RECURSE GRIB1_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp)
add_library(grib1API SHARED ${GRIB1_SOURCES})
add_library(gribAPI::v1 ALIAS grib1API)

find_package(Oster CONFIG REQUIRED)

target_include_directories(grib1API PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${OsterLib_INCLUDE_DIRS})
target_link_libraries(grib1API PRIVATE OsterLib::bytes OsterLib::serial OsterLib::utilities OsterLib::types OsterLib::boost_functional)
get_target_property(grib1API_INCLUDES grib1API INCLUDE_DIRECTORIES)
message(STATUS "Grib1 include directory: ${grib1API_INCLUDES}")
set(grib1API_INCLUDES ${grib1API_INCLUDES} PARENT_SCOPE)

add_subdirectory(tests)