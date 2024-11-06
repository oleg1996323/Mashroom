cmake_minimum_required(VERSION 3.15)
project(Mashroom_data_worker VERSION 0.1 LANGUAGES CXX C)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_C_COMPILER "/usr/bin/gcc")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE GRIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/grib/src/*.c)
file(GLOB_RECURSE Mashroom_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

message(STATUS ${Mashroom_SOURCES})


find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${CMAKE_CURRENT_LIST_DIR}/grib/include ${CMAKE_CURRENT_LIST_DIR}/include)

add_library(grib_c STATIC ${GRIB_SOURCES})

add_executable(Mashroom ${Mashroom_SOURCES})
set_target_properties(grib_c PROPERTIES CMAKE_C_COMPILER "/usr/bin/gcc")
set_target_properties(Mashroom PROPERTIES CMAKE_CXX_COMPILER "/usr/bin/g++")

target_link_libraries(Mashroom PRIVATE grib_c ${PNG_LIBRARIES})
target_include_directories(Mashroom PRIVATE ${CMAKE_CURRENT_LIST_DIR}/grib/include
                                        ${CMAKE_CURRENT_LIST_DIR}/grib/src
                                        ${CMAKE_CURRENT_LIST_DIR}/include
                                        ${CMAKE_CURRENT_LIST_DIR}/src
                                        ${PNG_INCLUDE_DIRS})

message(STATUS ${PNG_INCLUDE_DIRS})

set_target_properties(Mashroom grib_c
                    PROPERTIES
                    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/lib/Debug"
                    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/shared/Debug"
                    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/bin/Debug"
                    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/Release"
                    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/shared/Release"
                    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/bin/Release")