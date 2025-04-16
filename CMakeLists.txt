cmake_minimum_required(VERSION 3.22)
project(Mashroom_data_worker VERSION 0.1 LANGUAGES CXX)
set(LOG_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/log_error)
set(CONFIG_MR $ENV{HOME}/.config/Mashroom)
set(CMAKE_CXX_STANDARD 23)
add_compile_options("-O0")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external")
include(loadCodeTables)

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}/grib1/include/generated/code_tables")
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/eccodes")
execute_process(COMMAND "git" "submodule" "add" "https://github.com/ecmwf/eccodes.git" "${CMAKE_CURRENT_LIST_DIR}/external/eccodes")
endif()
load_eccode_header(
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/eccodes/definitions/grib1"
	OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/grib1"
    PYTHON_AUTOGEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external"
)

option(LOG_ON OFF)
option(PERMANENT_GET ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,$ENV{LD_LIBRARY_PATH}")
add_compile_options(-fPIC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lzip -static-libstdc++")

file(GLOB_RECURSE GRIB_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_LIST_DIR}/grib1/src/**.cpp)
file(GLOB_RECURSE Mashroom_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp)

find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

make_directory(third_party)
make_directory(third_party/png)
file(COPY ${PNG_LIBRARIES} DESTINATION third_party/png)
make_directory(third_party/libc)

include_directories(${CMAKE_CURRENT_LIST_DIR}/grib1/include 
                    ${CMAKE_CURRENT_LIST_DIR}/include 
                    ${CMAKE_CURRENT_LIST_DIR}/include/sys 
                    ${CMAKE_CURRENT_LIST_DIR}/include/cmd_parse)

add_executable(Mashroom ${Mashroom_SOURCES} ${GRIB_SOURCES})
add_library(Mashroom_lib SHARED ${Mashroom_SOURCES})

target_compile_definitions(Mashroom PRIVATE LOG_DIR="${LOG_DIRECTORY}" PRIVATE CONFIG_MASHROOM_DIR="${CONFIG_MR}" $<$<BOOL:${LOG_ON}>:LOG_ON> $<$<BOOL:${PERMANENT_GET}>:PERMANENT_GET> )
target_compile_definitions(Mashroom_lib PRIVATE LOG_DIR="${LOG_DIRECTORY}" PRIVATE CONFIG_MASHROOM_DIR="${CONFIG_MR}")
file(GLOB PNG_LIBS third_party/png/*.so)

target_include_directories(Mashroom PRIVATE ${CMAKE_CURRENT_LIST_DIR}/grib1/include
                                        ${CMAKE_CURRENT_LIST_DIR}/grib1/src
                                        ${CMAKE_CURRENT_LIST_DIR}/include
                                        ${CMAKE_CURRENT_LIST_DIR}/src
                                        ${PNG_INCLUDE_DIRS}
                                        ${CMAKE_CURRENT_LIST_DIR}/include/sys
                                        ${CMAKE_CURRENT_LIST_DIR}/include/cmd_parse)

target_link_libraries(Mashroom_lib PRIVATE ${PNG_LIBRARIES})
target_include_directories(Mashroom_lib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/grib1/include
                                        ${CMAKE_CURRENT_LIST_DIR}/grib1/src
                                        ${CMAKE_CURRENT_LIST_DIR}/include
                                        ${CMAKE_CURRENT_LIST_DIR}/src
                                        ${PNG_INCLUDE_DIRS}
                                        ${CMAKE_CURRENT_LIST_DIR}/include/sys
                                        ${CMAKE_CURRENT_LIST_DIR}/include/cmd_parse)
target_link_libraries(Mashroom PRIVATE Mashroom_lib ${PNG_LIBRARIES})
message(STATUS ${PNG_INCLUDE_DIRS})

set_target_properties(Mashroom Mashroom_lib
                    PROPERTIES
                    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/lib/Debug"
                    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/shared/Debug"
                    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/bin/Debug"
                    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/Release"
                    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/shared/Release"
                    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/bin/Release")