cmake_minimum_required(VERSION 3.16)
project("Common purpose API part" VERSION 0.0.1)

find_package(OsterLib CONFIG REQUIRED)

file(GLOB_RECURSE COMMON_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/**.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp")

add_library(common SHARED ${COMMON_SOURCES})
target_link_libraries(common 
        PRIVATE
        OsterLib::serial
        OsterLib::utilities
        OsterLib::types
        OsterLib::boost_functional)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                    PRIVATE ${OsterLib_INCLUDE_DIRS}
                                    )
if(${TESTS_ON})
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()