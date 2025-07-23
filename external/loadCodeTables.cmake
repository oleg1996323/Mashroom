function(load_eccode_header)
    set(options)
    set(oneValueArgs SOURCE_DIR OUTPUT_DIR PYTHON_AUTOGEN_DIR)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "" ${ARGN})

    # Проверка аргументов
    foreach(var IN ITEMS SOURCE_DIR OUTPUT_DIR PYTHON_AUTOGEN_DIR)
        if(NOT ARG_${var})
            message(FATAL_ERROR "Missing required argument: ${var}")
        endif()
    endforeach()

    find_package(Python3 REQUIRED)

    # Полные пути
    get_filename_component(FULL_PYTHON_SCRIPT "${ARG_PYTHON_AUTOGEN_DIR}/loadCodeTables.py" ABSOLUTE)
    get_filename_component(FULL_OUTPUT_DIR "${ARG_OUTPUT_DIR}" ABSOLUTE)
    get_filename_component(FULL_SOURCE_DIR "${ARG_SOURCE_DIR}" ABSOLUTE)

    # Выходные файлы (должны точно соответствовать тому, что генерирует скрипт)
    set(GENERATED_HEADER "${FULL_OUTPUT_DIR}/include/generated/code_tables/eccodes_tables.h")
    set(GENERATED_SOURCE "${FULL_OUTPUT_DIR}/src/generated/code_tables/eccodes_tables.cpp")

    # Создаем выходные директории, если их нет
    file(MAKE_DIRECTORY "${FULL_OUTPUT_DIR}/include/generated/code_tables")
    file(MAKE_DIRECTORY "${FULL_OUTPUT_DIR}/src/generated/code_tables")

    # Кастомная команда
    add_custom_command(
        OUTPUT ${GENERATED_HEADER} ${GENERATED_SOURCE}
        COMMAND ${PYTHON_EXECUTABLE} 
            "${FULL_PYTHON_SCRIPT}"
            "--src=${FULL_SOURCE_DIR}"
            "--out=${FULL_OUTPUT_DIR}"
        WORKING_DIRECTORY ${ARG_PYTHON_AUTOGEN_DIR}
        DEPENDS "${FULL_PYTHON_SCRIPT}"
        COMMENT "Generating ecCodes tables..."
        VERBATIM
    )

    # Цель генерации
    add_custom_target(
        generate_eccodes_tables ALL
        DEPENDS ${GENERATED_HEADER} ${GENERATED_SOURCE}
    )
endfunction()