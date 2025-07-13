file(GLOB_RECURSE GRIB1_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp)

add_library(grib1reader SHARED ${GRIB1_SOURCES})
target_include_directories(grib1reader PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)