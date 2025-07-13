cmake_minimum_required(VERSION 3.22)
project(Oster-Mashroom VERSION 0.1 LANGUAGES CXX)
find_package(Boost 1.75 REQUIRED COMPONENTS json program_options)

#configuration section
set(LOG_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/log_error)
set(CONFIG_REL_HOME_DIR .config/Mashroom)
set(APP_DATA_REL_DIR /data)
set(CMAKE_CXX_STANDARD 23)
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
	OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/grib1"
    PYTHON_AUTOGEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external"
)

#getting source files from modules
file(GLOB_RECURSE ALL_SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp)
file(GLOB_RECURSE CLparse ${CMAKE_SOURCE_DIR}/src/cmd_parse/**.cpp)
file(GLOB_RECURSE Network ${CMAKE_SOURCE_DIR}/src/network/**.cpp)
file(GLOB_RECURSE Mashroom_SOURCES ${CMAKE_SOURCE_DIR}/src/**.cpp)
list(FILTER Mashroom_SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/src/(network|cmd_parse)/")
list(FILTER Mashroom_SOURCES EXCLUDE REGEX "/main.cpp")

#including other build-modules
include(modules-linkage.cmake)

#build web part
add_library(web SHARED ${Network})
target_include_directories(web PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                                    ${CMAKE_CURRENT_SOURCE_DIR}/include/network 
                                    ${grib1_INCLUDES} ${library_INCLUDES})
target_link_libraries(web PUBLIC library)

#build command line interpreter parse
add_library(clip SHARED ${CLparse})
target_include_directories(clip PUBLIC 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                                    ${CMAKE_CURRENT_SOURCE_DIR}/include/cmd_parse 
                                    ${grib1_INCLUDES} ${library_INCLUDES})
target_link_libraries(clip PUBLIC library)

#build main lib
add_library(Mashroom_lib SHARED ${Mashroom_SOURCES})
target_link_libraries(Mashroom_lib PRIVATE clip web library grib1reader PRIVATE Boost::program_options Boost::json)
target_include_directories(Mashroom_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

add_executable(Mashroom ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_include_directories(Mashroom PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(Mashroom PRIVATE Mashroom_lib)

target_compile_definitions(Mashroom PRIVATE LOG_DIR="${LOG_DIRECTORY}" PRIVATE CONFIG_MASHROOM_DIR_REL="${CONFIG_REL_HOME_DIR}" $<$<BOOL:${LOG_ON}>:LOG_ON> $<$<BOOL:${PERMANENT_GET}>:PERMANENT_GET> 
                        APP_DATA_RELATION_DIR="${APP_DATA_REL_DIR}")
target_compile_definitions(Mashroom_lib PRIVATE LOG_DIR="${LOG_DIRECTORY}" PRIVATE CONFIG_MASHROOM_DIR_REL="${CONFIG_REL_HOME_DIR}")                
                    
set_target_properties(Mashroom Mashroom_lib grib1reader library clip web
                    PROPERTIES
                    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/lib/Debug"
                    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/shared/Debug"
                    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/bin/Debug"
                    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/Release"
                    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/shared/Release"
                    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/bin/Release")