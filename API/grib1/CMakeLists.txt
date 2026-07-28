cmake_minimum_required(VERSION 3.16)
project("GRIB v1 API" VERSION 0.0.1)

#execute autogeneration of code tables
if(NOT EXISTS "${API_EXTERNALS_DIR}/eccodes")
    execute_process(COMMAND "git" "submodule" "add" "https://github.com/ecmwf/eccodes.git" "${API_EXTERNALS_DIR}/eccodes")
else()
    execute_process(COMMAND "git" "submodule" "update" "${API_EXTERNALS_DIR}/eccodes")
endif()

add_executable(autogen ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_compile_definitions(autogen PRIVATE EXE_PATH="$<TARGET_FILE:autogen>")
target_include_directories(autogen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set(ORG_DAT "${CMAKE_CURRENT_SOURCE_DIR}/organizations.dat")
set(ORG_TMP "${ORG_DAT}.tmp")

add_custom_command(
    OUTPUT ${ORG_DAT}
    COMMAND "$<TARGET_FILE:autogen>" ${ORG_TMP}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ORG_TMP} ${ORG_DAT}
    COMMAND ${CMAKE_COMMAND} -E remove ${ORG_TMP}
    DEPENDS "$<TARGET_FILE:autogen>"
    COMMENT "Generating organizations.dat"
)

find_package(Python3 REQUIRED)

set(TABLE_0_H "${CMAKE_CURRENT_SOURCE_DIR}/include/grib1/generated/code_tables/table_0.h")
add_custom_command(
    OUTPUT ${TABLE_0_H}
    COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/organizations_gen.py"
            "--src=${CMAKE_CURRENT_SOURCE_DIR}"
            "--out=${CMAKE_CURRENT_SOURCE_DIR}/include/grib1/generated/code_tables"
    DEPENDS ${ORG_DAT} "${CMAKE_CURRENT_SOURCE_DIR}/organizations_gen.py"
    COMMENT "Generating table code 0 header for organizations"
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
#including autogeneration code (code_tables)
include(loadCodeTables)
load_eccode_header(
	SOURCE_DIR "${API_EXTERNALS_DIR}/eccodes/definitions/grib1"
	OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/grib1/generated/code_tables"
    PYTHON_AUTOGEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_compile_definitions(${GRIB1API})
file(GLOB_RECURSE GRIB1_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/**.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp")

find_package(OsterLib CONFIG REQUIRED)
add_library(grib1API SHARED ${GRIB1_SOURCES})
add_library(gribAPI::v1 ALIAS grib1API)

if(TARGET gribAPI::v1)
    message(STATUS "gribAPI::v1 exists")
else()
    message(FATAL "gribAPI::v1 don't exists")
endif()

add_dependencies(grib1API autogen generate_grib1_eccodes_tables)
target_sources(grib1API PRIVATE ${GENERATED_SOURCE} ${TABLE_0_H})

target_link_libraries(grib1API 
        PUBLIC common 
        PRIVATE OsterLib::bytes
        OsterLib::serial
        OsterLib::utilities
        OsterLib::types
        OsterLib::boost_functional)
target_include_directories(grib1API 
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE ${OsterLib_INCLUDE_DIRS})

if(${TESTS_ON})
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()