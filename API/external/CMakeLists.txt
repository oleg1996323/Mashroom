add_executable(autogen ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_compile_definitions(autogen PRIVATE ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}" EXE_PATH="$<TARGET_FILE:autogen>")
target_include_directories(autogen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(
    organizations ALL
    DEPENDS autogen
    COMMAND ${CMAKE_COMMAND} -E echo "Running autogen..."
    COMMAND "$<TARGET_FILE:autogen>"  # Используем генераторное выражение напрямую
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}  # Запускаем в нужной директории
    COMMENT "Generating organizations.dat"
    BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/organizations.dat
    VERBATIM
)

find_package(Python3 REQUIRED)

add_custom_target(
    table_org ALL
    DEPENDS organizations "${CMAKE_CURRENT_SOURCE_DIR}/organizations_gen.py"
    COMMAND ${Python3_EXECUTABLE} 
            "${CMAKE_CURRENT_SOURCE_DIR}/organizations_gen.py"
            "--src=${CMAKE_CURRENT_SOURCE_DIR}"
            "--out=${CMAKE_SOURCE_DIR}/API/grib1/include/generated/code_tables"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}  # Запускаем в нужной директории
    COMMENT "Generating table code 0 header for organizations"
    BYPRODUCTS "${CMAKE_SOURCE_DIR}/API/grib1/include/generated/code_tables/table_0.h"
    VERBATIM
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
#including autogeneration code (code_tables)
include(loadCodeTables)
load_eccode_header(
	SOURCE_DIR "${CMAKE_SOURCE_DIR}/API/external/eccodes/definitions/grib1"
	OUTPUT_DIR "${CMAKE_SOURCE_DIR}/API/grib1"
    PYTHON_AUTOGEN_DIR "${CMAKE_SOURCE_DIR}/API/external"
)