add_executable(autogen ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

#execute autogeneration of code tables
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/eccodes")
    execute_process(COMMAND "git" "submodule" "add" "https://github.com/ecmwf/eccodes.git" "eccodes")
else()
    execute_process(COMMAND "git" "submodule" "update" "eccodes")
endif()

target_compile_definitions(autogen PRIVATE ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}" EXE_PATH="$<TARGET_FILE:autogen>")
target_include_directories(autogen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set(ORG_DAT "${CMAKE_CURRENT_SOURCE_DIR}/organizations.dat")
set(ORG_TMP "${ORG_DAT}.tmp")

add_custom_command(
    OUTPUT ${ORG_DAT}
    COMMAND "$<TARGET_FILE:autogen>" > ${ORG_TMP}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ORG_TMP} ${ORG_DAT}
    COMMAND ${CMAKE_COMMAND} -E remove ${ORG_TMP}
    DEPENDS autogen
    COMMENT "Generating organizations.dat"
)

find_package(Python3 REQUIRED)

set(TABLE_0_H "${CMAKE_SOURCE_DIR}/API/grib1/include/generated/code_tables/table_0.h")
add_custom_command(
    OUTPUT ${TABLE_0_H}
    COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/organizations_gen.py"
            "--src=${CMAKE_CURRENT_SOURCE_DIR}"
            "--out=${CMAKE_SOURCE_DIR}/API/grib1/include/generated/code_tables"
    DEPENDS ${ORG_DAT} "${CMAKE_CURRENT_SOURCE_DIR}/organizations_gen.py"
    COMMENT "Generating table code 0 header for organizations"
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
#including autogeneration code (code_tables)
include(loadCodeTables)
load_eccode_header(
	SOURCE_DIR "${CMAKE_SOURCE_DIR}/API/external/eccodes/definitions/grib1"
	OUTPUT_DIR "${CMAKE_SOURCE_DIR}/API/grib1"
    PYTHON_AUTOGEN_DIR "${CMAKE_SOURCE_DIR}/API/external"
)