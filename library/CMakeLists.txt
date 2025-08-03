cmake_minimum_required(VERSION 3.16)
project(Oster-Lib
        VERSION 0.1
        DESCRIPTION "General purpose library with essential utilities"
        LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Boost CONFIG 1.82 REQUIRED json)
file(GLOB_RECURSE LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp)
file(GLOB_RECURSE LINENOISE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/linenoise/linenoise.c)

add_library(serial INTERFACE)
add_library(Osterlib::serial ALIAS serial)
target_sources(serial PUBLIC 
                include/serialization.h
                include/byte_order.h
                include/byte_read.h
                include/concepts.h)
target_include_directories(serial INTERFACE include)


add_library(bytes INTERFACE)
add_library(Osterlib::bytes ALIAS bytes)
target_sources(bytes PUBLIC
                include/byte_order.h
                include/byte_read.h)
target_include_directories(bytes INTERFACE include)

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)
add_library(lineNoise STATIC ${LINENOISE_SOURCES})

add_library(library STATIC ${LIB_SOURCES})
target_link_libraries(library PUBLIC Boost::json PRIVATE lineNoise ${ZLIB_LIBRARIES} ${PNG_LIBRARIES})
target_include_directories(library PUBLIC ${CMAKE_CURRENT_LIST_DIR} 
                                            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
                                            $<INSTALL_INTERFACE:include>
                                            PRIVATE ${CMAKE_CURRENT_LIST_DIR}/linenoise ${PNG_INCLUDE_DIRS})

# set_target_properties(lineNoise PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/lib/Debug"
#                                            ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/Release")

get_target_property(library_INCLUDES library INCLUDE_DIRECTORIES)
set(library_INCLUDES ${library_INCLUDES} PARENT_SCOPE)
set(library_SOURCES ${LIB_SOURCES} PARENT_SCOPE)
set(library_LIB ${LIB_SOURCES} PARENT_SCOPE)

add_subdirectory(tests)