cmake_minimum_required(VERSION 3.22)
project("Mashroom serialization tests")

enable_testing()

add_executable(Mashroom_serialize_test mashroom_serialization.cpp)
target_link_libraries(Mashroom_serialize_test GTest::gtest Mashroom_lib)
message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
gtest_discover_tests(Mashroom_serialize_test)
set_target_properties(Mashroom_serialize_test PROPERTIES EXCLUDE_FROM_ALL FALSE)