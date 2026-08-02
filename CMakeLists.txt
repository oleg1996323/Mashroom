cmake_minimum_required(VERSION 3.22)
project(Oster-Mashroom
        VERSION 0.1
        DESCRIPTION "Universal indexer, effective extractor, handler and network-sharer of different geoinformational data"
        LANGUAGES C CXX)
set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
find_package(Boost 1.75 CONFIG REQUIRED COMPONENTS json program_options)
find_package(GTest CONFIG REQUIRED)
set(CMAKE_SKIP_TEST_ALL_DEPENDENCY FALSE)
#configuration section

#add submodules
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/external/linenoise")
    execute_process(COMMAND "git" "submodule" "add" "--force" "https://github.com/antirez/linenoise" "external/linenoise")
endif()

find_package(CLI11 CONFIG REQUIRED)

set(CONFIG_REL_HOME_DIR .config/Mashroom)
set(NETWORK_FILES_DIR network)
set(SYSTEM_FILES_DIR system)
set(LOG_DIRECTORY log)
set(APP_DATA_REL_DIR /data)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
option(LOG_ON OFF)
option(PERMANENT_GET ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,$ENV{LD_LIBRARY_PATH}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lzip -static-libstdc++")
add_compile_options("-fPIC" "-O0" "-fconcepts-diagnostics-depth=10" "-fdiagnostics-all-candidates")

#getting source files from modules
file(GLOB_RECURSE ALL_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp)
file(GLOB_RECURSE CLparse_SOURCES ${CMAKE_SOURCE_DIR}/src/cmd_parse/**.cpp)
file(GLOB_RECURSE Network_SOURCES ${CMAKE_SOURCE_DIR}/src/web/**.cpp)
file(GLOB_RECURSE Sys_SOURCES ${CMAKE_SOURCE_DIR}/src/sys/**.cpp)
file(GLOB_RECURSE Mashroom_SOURCES ${CMAKE_SOURCE_DIR}/src/**.cpp)
list(FILTER Mashroom_SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/src/(web|cmd_parse|sys)/*")
message(STATUS "System sources: ${Sys_SOURCES}")
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
message(STATUS ${OSTERLIB_PATH})
find_package(OsterLib CONFIG REQUIRED PATHS ${OSTERLIB_PATH})
add_subdirectory(API)

execute_process(COMMAND "git submodule update --remote --recursive")
#build system module
add_library(system SHARED ${Sys_SOURCES})
target_include_directories(system PRIVATE ${CMAKE_SOURCE_DIR}/include/sys ${CMAKE_SOURCE_DIR}/include ${OsterLib_INCLUDE_DIRS})
target_compile_definitions(system PUBLIC LOG_DIR="${LOG_DIRECTORY}" CONFIG_MASHROOM_DIR_REL="${CONFIG_REL_HOME_DIR}"
        NETWORK_MASHROOM_FILES_DIR="${NETWORK_FILES_DIR}" SYSTEM_MASHROOM_FILES_DIR)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(STATUS "DEBUG defined")
    target_compile_definitions(system PUBLIC DEBUG)
endif()
target_link_libraries(system PUBLIC OsterLib::boost_functional OsterLib::network)
#build web part
add_library(web SHARED ${Network_SOURCES})
target_include_directories(web PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                                    PRIVATE
                                    ${OsterLib_INCLUDE_DIRS})

target_link_libraries(web PUBLIC OsterLib::boost_functional OsterLib::types API_lib OsterLib::network system)

#build command line interpreter parse (CLIp)
add_library(clip SHARED ${CLparse_SOURCES})
target_include_directories(clip PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                                    ${CMAKE_CURRENT_SOURCE_DIR}/include/cmd_parse
                                    PRIVATE
                                    ${OsterLib_INCLUDE_DIRS})
target_link_libraries(clip PUBLIC API_lib OsterLib::types OsterLib::CLInput system Boost::json CLI11::CLI11 web)

#build main lib
add_library(Mashroom_lib SHARED ${Mashroom_SOURCES})
target_link_libraries(Mashroom_lib PUBLIC API_lib
                    system
                    clip
                    web
                    OsterLib::boost_functional
                    OsterLib::utilities
                    OsterLib::compressor
                    OsterLib::types)
target_include_directories(Mashroom_lib 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR})

get_target_property(MASHROOM_INC Mashroom_lib INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "Mashroom include directories: ${MASHROOM_INC}")

if(NOT TARGET API_lib)
    message(FATAL "API_lib don't exists")
else()
    message(STATUS "API_lib exists")
endif()

get_target_property(API_lib_INCLUDES API_lib INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "API_lib includes: ${API_lib_INCLUDES}")

add_executable(Mashroom ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(Mashroom PRIVATE Mashroom_lib)
target_include_directories(Mashroom PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(Mashroom PRIVATE APP_DATA_RELATION_DIR="${APP_DATA_REL_DIR}")

set(CMAKE_LIBRARY_PATH ${CMAKE_BINARY_DIR}/Debug/shared)
enable_testing()
add_subdirectory(tests)