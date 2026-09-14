cmake_minimum_required(VERSION 3.16)
project("common tests")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
enable_testing()
add_compile_options("-O0")
find_package(Boost 1.91 CONFIG REQUIRED COMPONENTS json program_options)
find_package(geos REQUIRED)
find_package(GTest CONFIG REQUIRED)
get_target_property(dirs GEOS::geos_c INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS ${dirs})
get_target_property(COMMON_DIRS common INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS ${COMMON_DIRS})
add_executable(polygon_test polygon_test.cpp)
target_link_libraries(polygon_test PRIVATE OsterLib::types OsterLib::boost_functional GTest::gtest GEOS::geos_c)

target_include_directories(polygon_test PUBLIC ${COMMON_DIRS})
target_compile_definitions(polygon_test PRIVATE BOOST_CSTDFLOAT_FLOAT128_QUIET_NAN)
target_compile_options(polygon_test PRIVATE "-O0")
message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
gtest_discover_tests(
        polygon_test 
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
