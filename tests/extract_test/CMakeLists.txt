cmake_minimum_required(VERSION 3.22)
project(extract_test)

enable_testing()

add_executable(bin_test bin_test.cpp)
add_executable(txt_test txt_test.cpp)
add_executable(json_test json_test.cpp)

target_link_libraries(bin_test PUBLIC GTest::gtest Mashroom_lib)
target_link_libraries(txt_test PUBLIC GTest::gtest Mashroom_lib)
target_link_libraries(json_test PUBLIC GTest::gtest Mashroom_lib)

gtest_discover_tests(bin_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
gtest_discover_tests(txt_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
gtest_discover_tests(json_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})