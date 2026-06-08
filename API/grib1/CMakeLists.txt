cmake_minimum_required(VERSION 3.16)
project("GRIB v1 API" VERSION 0.0.1)
file(GLOB_RECURSE GRIB1_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp)
add_library(grib1API SHARED ${GRIB1_SOURCES})
add_library(gribAPI::v1 ALIAS grib1API)

find_package(OsterLib CONFIG REQUIRED)

target_include_directories(grib1API PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                            ${OsterLib_INCLUDE_DIRS}
                                            PRIVATE ${API_DIR})
target_link_libraries(grib1API PRIVATE OsterLib::bytes OsterLib::serial OsterLib::utilities OsterLib::types OsterLib::boost_functional)

set(grib1API_INCLUDES ${grib1API_INCLUDES} PARENT_SCOPE)
target_sources(grib1API PRIVATE ${GENERATED_SOURCE})
target_include_directories(grib1API PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_dependencies(grib1API generate_grib1_eccodes_tables)
add_subdirectory(tests)