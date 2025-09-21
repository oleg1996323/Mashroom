cmake_minimum_required(VERSION 3.22)

project("Network library tests")

enable_testing()
find_package(GTest CONFIG REQUIRED)
add_executable(network_lib_test ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)
target_include_directories(network_lib_test PRIVATE ${OsterLib_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(network_lib_test PRIVATE network_lib GTest::gtest)
message(STATUS "Current bin: ${CMAKE_CURRENT_BINARY_DIR}")
gtest_discover_tests(network_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})