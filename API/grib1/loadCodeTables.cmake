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
    set(GENERATED_HEADER "${FULL_OUTPUT_DIR}/eccodes_tables.h")
    set(GENERATED_SOURCE "${FULL_OUTPUT_DIR}/eccodes_tables.cpp")

    # Создаем выходные директории, если их нет
    file(MAKE_DIRECTORY "${FULL_OUTPUT_DIR}")
    file(MAKE_DIRECTORY "${FULL_OUTPUT_DIR}")

    file(GLOB TABLE_FILES "${ARG_SOURCE_DIR}/*.table")

    add_custom_command(
        OUTPUT ${GENERATED_HEADER} ${GENERATED_SOURCE}
        COMMAND ${Python3_EXECUTABLE}
            "${ARG_PYTHON_AUTOGEN_DIR}/loadCodeTables.py"
            "--src=${ARG_SOURCE_DIR}"
            "--out=${ARG_OUTPUT_DIR}"
        DEPENDS ${TABLE_FILES} "${ARG_PYTHON_AUTOGEN_DIR}/loadCodeTables.py"
        COMMENT "Generating ecCodes tables..."
        VERBATIM
    )
    add_custom_target(generate_grib1_eccodes_tables DEPENDS ${GENERATED_HEADER} ${GENERATED_SOURCE})
endfunction()