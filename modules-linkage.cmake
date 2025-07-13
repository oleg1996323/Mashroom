#including other build-modules
include("${CMAKE_SOURCE_DIR}/library/CMakeLists.txt")
include("${CMAKE_SOURCE_DIR}/grib1/grib1reader.cmake")
target_link_libraries(grib1reader PUBLIC library)

get_target_property(grib1reader_INCLUDES grib1reader INCLUDE_DIRECTORIES)
get_target_property(library_INCLUDES library INCLUDE_DIRECTORIES)

