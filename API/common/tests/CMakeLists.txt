cmake_minimum_required(VERSION 3.16)
project("common tests")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
enable_testing()
add_compile_options("-O0")
find_package(GTest CONFIG REQUIRED)
get_target_property(COMMON_DIRS common INTERFACE_INCLUDE_DIRECTORIES)

add_executable(polygon_test polygon_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../src/geometry/Polygon.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/geometry/algorithms.cpp)
add_executable(line_test line_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../src/geometry/Line.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/geometry/algorithms.cpp)
add_executable(polyline_test polyline_test.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../src/geometry/Polyline.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/geometry/algorithms.cpp)

target_link_libraries(polygon_test PRIVATE OsterLib::types GTest::gtest)
target_include_directories(polygon_test PUBLIC ${COMMON_DIRS})
target_compile_definitions(polygon_test PRIVATE BOOST_CSTDFLOAT_FLOAT128_QUIET_NAN)
target_compile_options(polygon_test PRIVATE "-O0")

target_link_libraries(line_test PRIVATE OsterLib::types GTest::gtest)
target_include_directories(line_test PUBLIC ${COMMON_DIRS})
target_compile_definitions(line_test PRIVATE BOOST_CSTDFLOAT_FLOAT128_QUIET_NAN)
target_compile_options(line_test PRIVATE "-O0")

target_link_libraries(polyline_test PRIVATE OsterLib::types GTest::gtest)
target_include_directories(polyline_test PUBLIC ${COMMON_DIRS})
target_compile_definitions(polyline_test PRIVATE BOOST_CSTDFLOAT_FLOAT128_QUIET_NAN)
target_compile_options(polyline_test PRIVATE "-O0")
message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
gtest_discover_tests(
        polygon_test 
        line_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
