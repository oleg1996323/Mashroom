file(GLOB_RECURSE GRIB1_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp)

find_package(GeographicLib REQUIRED)

add_library(grib1reader SHARED ${GRIB1_SOURCES})
target_include_directories(grib1reader PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include ${GeographicLib_INCLUDES})
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(grib1reader PRIVATE ${GeographicLib_LIBRARIES})