cmake_minimum_required(VERSION 3.16)
project("common tests")
enable_testing()

get_target_property(COMMON_DIRS common INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS ${COMMON_DIRS})
add_executable(polygon_test polygon_test.cpp)
target_link_libraries(polygon_test PRIVATE OsterLib::types GTest::gtest)

target_include_directories(polygon_test PUBLIC ${COMMON_DIRS})

message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
gtest_discover_tests(
        polygon_test 
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})