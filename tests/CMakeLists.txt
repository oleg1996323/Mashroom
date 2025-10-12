cmake_minimum_required(VERSION 3.22)
project("Mashroom serialization tests")

enable_testing()

add_executable(Mashroom_serialize_test mashroom_serialization.cpp)
add_executable(Mashroom_messages_test mashroom_messages.cpp)
target_link_libraries(Mashroom_serialize_test INTERFACE GIAPI PUBLIC GTest::gtest Mashroom_lib)
target_link_libraries(Mashroom_messages_test INTERFACE GIAPI PUBLIC GTest::gtest Mashroom_lib)
message(STATUS "Current bin: ${CMAKE_CURRENT_BINARY_DIR}")
gtest_discover_tests(Mashroom_serialize_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
gtest_discover_tests(Mashroom_messages_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})