function(load_eccode_header)
	set(options)
	set(oneValueArgs SOURCE_DIR OUTPUT_DIR)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "" ${ARGN})

	# Ищем Python
	find_package(PythonInterp 3.6 REQUIRED)

	# Создаем кастомную команду
	message(STATUS "OUTPUT_DIR: " ${ARG_OUTPUT_DIR} "\n" "SOURCE_DIR: " ${ARG_SOURCE_DIR})
	add_custom_command(
		OUTPUT "${ARG_OUTPUT_DIR}/eccodes_tables.h"
		COMMAND ${PYTHON_EXECUTABLE} 
			"${CMAKE_CURRENT_SOURCE_DIR}/external/loadCodeTables.py"
			"--src=${ARG_SOURCE_DIR}"
			"--out=${ARG_OUTPUT_DIR}"
		DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/external/loadCodeTables.py"
		COMMENT "Load ecCodes tables..."
	)

	# Кастомный таргет для удобства
	add_custom_target(
		loadCodeTables ALL
		DEPENDS "${ARG_OUTPUT_DIR}/eccodes_tables.h"
	)
endfunction()