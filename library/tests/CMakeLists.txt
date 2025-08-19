cmake_minimum_required(VERSION 3.16)
project(tests)
enable_testing()

add_executable(serialization_test serialization.cpp)
target_link_libraries(serialization_test GTest::gtest Osterlib::serial)

add_executable(bytes_test bytes.cpp)
target_link_libraries(bytes_test GTest::gtest Osterlib::bytes)

gtest_discover_tests(bytes_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
gtest_discover_tests(serialization_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
# gtest_discover_tests(serialization_test 
#                         EXTRA_ARGS serialization_test 
#                         XML_OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/tests/results
#                         AUTO)