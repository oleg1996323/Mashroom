cmake_minimum_required(VERSION 3.16)
project("GRIB v1 API tests")
enable_testing()

add_executable(grid_test grid_test.cpp)
target_link_libraries(grid_test PRIVATE gribAPI::v1 GTest::gtest)

message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
gtest_discover_tests(grid_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})