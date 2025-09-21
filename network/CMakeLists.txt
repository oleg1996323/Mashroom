cmake_minimum_required(VERSION 3.22)
project("Network library")

find_package(OsterLib REQUIRED)
add_library(network_lib SHARED ${Network_lib_SOURCES})
target_include_directories(network_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(network_lib PUBLIC OsterLib::serial OsterLib::bytes)
add_subdirectory(test)