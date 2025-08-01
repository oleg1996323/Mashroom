cmake_minimum_required(VERSION 3.16)
project(Oster-Lib)
set(CMAKE_CXX_STANDARD 23)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Boost 1.82 REQUIRED json)
file(GLOB_RECURSE LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp)
file(GLOB_RECURSE LINENOISE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/linenoise/linenoise.c)
add_library(lineNoise STATIC ${LINENOISE_SOURCES})

add_library(library STATIC ${LIB_SOURCES})
target_link_libraries(library PUBLIC ${ZLIB_LIBRARIES} ${PNG_LIBRARIES} Boost::json PRIVATE lineNoise)
target_include_directories(library PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/include PRIVATE ${CMAKE_CURRENT_LIST_DIR}/linenoise)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include ${PNG_INCLUDE_DIRS})


set_target_properties(lineNoise PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/lib/Debug"
                                           ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/Release")


add_executable(serialization ${CMAKE_CURRENT_LIST_DIR}/tests/serialization.cpp)