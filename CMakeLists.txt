cmake_minimum_required(VERSION 3.22)
project(Oster-Mashroom
        VERSION 0.1
        DESCRIPTION "Universal indexer, effective extractor, handler and network-sharer of different geoinformational data"
        LANGUAGES C CXX)
enable_testing()
set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
find_package(Boost 1.75 CONFIG REQUIRED COMPONENTS json program_options)
set(CMAKE_SKIP_TEST_ALL_DEPENDENCY FALSE)
#configuration section
set(LOG_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/log_error)
set(CONFIG_REL_HOME_DIR .config/Mashroom)
set(APP_DATA_REL_DIR /data)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
option(LOG_ON OFF)
option(PERMANENT_GET ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,$ENV{LD_LIBRARY_PATH}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lzip -static-libstdc++")
add_compile_options("-fPIC" "-O0" "-fconcepts-diagnostics-depth=10")
#adding imported submodules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external")

#including autogeneration code (code_tables)
include(autogen)
include(loadCodeTables)
#execute autogeneration of code tables
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/external/eccodes")
    execute_process(COMMAND "git" "submodule" "add" "https://github.com/ecmwf/eccodes.git" "${CMAKE_CURRENT_LIST_DIR}/external/eccodes")
endif()
load_eccode_header(
	SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/eccodes/definitions/grib1"
	OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/API/grib1"
    PYTHON_AUTOGEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external"
)

#getting source files from modules
file(GLOB_RECURSE ALL_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp)
file(GLOB_RECURSE CLparse_SOURCES ${CMAKE_SOURCE_DIR}/src/cmd_parse/**.cpp)
file(GLOB_RECURSE Network_SOURCES ${CMAKE_SOURCE_DIR}/src/network/**.cpp)
file(GLOB_RECURSE Sys_SOURCES ${CMAKE_SOURCE_DIR}/src/sys/**.cpp)
file(GLOB_RECURSE Mashroom_SOURCES ${CMAKE_SOURCE_DIR}/src/**.cpp)
list(FILTER Mashroom_SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/src/(network|cmd_parse|sys)/")
list(FILTER Mashroom_SOURCES EXCLUDE REGEX "/main.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug/shared)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/shared)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG})
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE})
endif()

set(CTEST_BINARY_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests)
set(CTEST_SCRIPT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests)
set(CTEST_SOURCE_DIRECTORY ${CMAKE_BINARY_DIR})

#including other build-modules
find_package(GTest CONFIG REQUIRED)

find_package(OsterLib CONFIG REQUIRED)
add_subdirectory(API)

#build system module
add_library(system SHARED ${Sys_SOURCES})
target_include_directories(system PUBLIC ${CMAKE_SOURCE_DIR}/include/sys ${CMAKE_SOURCE_DIR}/include ${OsterLib_INCLUDE_DIRS})
target_compile_definitions(system PUBLIC LOG_DIR="${LOG_DIRECTORY}" CONFIG_MASHROOM_DIR_REL="${CONFIG_REL_HOME_DIR}")
target_link_libraries(system PUBLIC OsterLib::bytes OsterLib::serial)
#build web part
add_library(web SHARED ${Network_SOURCES})
target_include_directories(web PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                                    ${CMAKE_CURRENT_SOURCE_DIR}/include/network
                                    ${OsterLib_INCLUDE_DIRS})

target_link_libraries(web PUBLIC OsterLib::boost_functional GIAPI system)

#build command line interpreter parse (CLIp)
add_library(clip SHARED ${CLparse_SOURCES})
target_include_directories(clip PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                                    ${CMAKE_CURRENT_SOURCE_DIR}/include/cmd_parse
                                    ${CMAKE_CURRENT_SOURCE_DIR}/include/cmd_parse/types_parse
                                    ${OsterLib_INCLUDE_DIRS})
target_link_libraries(clip PUBLIC GIAPI OsterLib::CLInput system Boost::program_options Boost::json)

#build main lib
add_library(Mashroom_lib SHARED ${Mashroom_SOURCES})
target_link_libraries(Mashroom_lib PUBLIC GIAPI
                    system
                    clip
                    web
                    OsterLib::serial
                    OsterLib::boost_functional
                    OsterLib::bytes
                    OsterLib::utilities
                    OsterLib::compressor
                    OsterLib::types)
target_include_directories(Mashroom_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include Oster_INCLUDE_DIR)

add_executable(Mashroom ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(Mashroom PRIVATE Mashroom_lib)
target_include_directories(Mashroom PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(Mashroom PRIVATE APP_DATA_RELATION_DIR="${APP_DATA_REL_DIR}")

set(CMAKE_LIBRARY_PATH ${CMAKE_BINARY_DIR}/Debug/shared)

add_subdirectory(tests)