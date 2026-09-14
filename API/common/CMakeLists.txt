cmake_minimum_required(VERSION 3.26)
project("Common purpose API part" VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)

find_package(OsterLib CONFIG REQUIRED)

file(GLOB_RECURSE COMMON_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/**.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp")

file(GLOB_RECURSE DATUM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/datum/**.cpp")
add_library(datum STATIC ${DATUM_SOURCES})
target_include_directories(datum PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                    ${OsterLib_INCLUDE_DIRS}
                                    )

file(GLOB_RECURSE PROJECTION_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/coordinate_system/**.cpp")
add_library(csys STATIC ${PROJECTION_SOURCES})
target_link_libraries(csys PUBLIC datum OsterLib::types)
target_include_directories(csys PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                    ${OsterLib_INCLUDE_DIRS}
                                    )

file(GLOB_RECURSE PROJECTION_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/projection/**.cpp")
add_library(projection STATIC ${PROJECTION_SOURCES})
target_link_libraries(projection PUBLIC csys OsterLib::types)
target_include_directories(projection PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                    ${OsterLib_INCLUDE_DIRS}
                                    )

file(GLOB_RECURSE PROJECTION_AREA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/projection_area/**.cpp")
add_library(projection_area STATIC ${PROJECTION_AREA_SOURCES})
target_link_libraries(projection_area PUBLIC projection datum OsterLib::types)
target_include_directories(projection_area PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                    ${OsterLib_INCLUDE_DIRS}
                                    )
file(GLOB_RECURSE GRID_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/grid/**.cpp")
add_library(grid STATIC ${GRID_SOURCES})
target_link_libraries(grid PUBLIC projection_area OsterLib::types)
target_include_directories(grid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                    ${OsterLib_INCLUDE_DIRS}
                                    )

add_library(common SHARED ${COMMON_SOURCES})
target_link_libraries(common 
        PRIVATE
        OsterLib::utilities
        OsterLib::types
        OsterLib::boost_functional)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                    ${OsterLib_INCLUDE_DIRS}
                                    )
if(${TESTS_ON})
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()