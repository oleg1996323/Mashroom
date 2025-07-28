add_executable(autogen ${CMAKE_CURRENT_LIST_DIR}/main.cpp)

target_compile_definitions(autogen PRIVATE ROOT_DIR="${CMAKE_CURRENT_LIST_DIR}" EXE_DIR="${AUTOGEN_EXECUTABLE_DIR}" EXE_PATH="$<TARGET_FILE:autogen>")
target_include_directories(autogen PRIVATE ${CMAKE_CURRENT_LIST_DIR})
set_target_properties(autogen
                        PROPERTIES
                        ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/lib/Debug"
                        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/shared/Debug"
                        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/bin/Debug"
                        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/Release"
                        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/shared/Release"
                        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/bin/Release")



set(AUTOGEN_EXECUTABLE_DIR "$<TARGET_FILE_DIR:autogen>")

message(STATUS "$<TARGET_FILE_DIR:autogen>/autogen")

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_LIST_DIR}/organizations.dat
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/main.cpp ${CMAKE_CURRENT_LIST_DIR}/table_0.h
    COMMAND "$<TARGET_FILE:autogen>"  # Используем генераторное выражение напрямую
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}  # Запускаем в нужной директории
    COMMENT "Generating organizations.dat"
    BYPRODUCTS ${CMAKE_CURRENT_LIST_DIR}/organizations.dat
)

find_package(Python3 REQUIRED)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/grib1/include/generated/code_tables/table_0.h"
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/organizations.dat "${CMAKE_CURRENT_LIST_DIR}/organizations_gen.py"
    COMMAND ${Python3_EXECUTABLE} 
            "${CMAKE_CURRENT_LIST_DIR}/organizations_gen.py"
            "--src=${CMAKE_CURRENT_LIST_DIR}"
            "--out=${CMAKE_CURRENT_SOURCE_DIR}/grib1/include/generated/code_tables"
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}  # Запускаем в нужной директории
    COMMENT "Generating table code 0 header for organizations"
    VERBATIM
)

# Цель генерации
add_custom_target(
    generated_table_0 ALL
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/grib1/include/generated/code_tables/table_0.h"
)