cmake_minimum_required(VERSION 4.0)
project(tests)
enable_testing()
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)
message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
add_executable(serialization_test serialization.cpp)
target_link_libraries(serialization_test GTest::gtest Osterlib::serial)

add_executable(bytes_test bytes.cpp)
target_link_libraries(bytes_test GTest::gtest Osterlib::bytes)

gtest_discover_tests(bytes_test)
gtest_discover_tests(serialization_test)
# gtest_discover_tests(serialization_test 
#                         EXTRA_ARGS serialization_test 
#                         XML_OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/tests/results
#                         AUTO)