cmake_minimum_required(VERSION 3.22)
project(Mashroom_data_worker VERSION 0.1 LANGUAGES CXX C)
set(LOG_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/log_error)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,$ENV{LD_LIBRARY_PATH}")
add_compile_options(-fPIC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lzip -static-libgcc -static-libstdc++")

file(GLOB_RECURSE GRIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/grib/src/**.c)
file(GLOB_RECURSE Mashroom_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.cpp)


find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

make_directory(third_party)
make_directory(third_party/png)
file(COPY ${PNG_LIBRARIES} DESTINATION third_party/png)
make_directory(third_party/libc)

include_directories(${CMAKE_CURRENT_LIST_DIR}/grib/include ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/sys)

add_library(grib_c STATIC ${GRIB_SOURCES})

add_executable(Mashroom ${Mashroom_SOURCES})
add_library(Mashroom_lib SHARED ${Mashroom_SOURCES})

target_compile_definitions(Mashroom PRIVATE LOG_DIR="${LOG_DIRECTORY}")
target_compile_definitions(Mashroom_lib PRIVATE LOG_DIR="${LOG_DIRECTORY}")
file(GLOB PNG_LIBS third_party/png/*.so)

target_link_libraries(Mashroom PRIVATE grib_c ${PNG_LIBRARIES})
target_include_directories(Mashroom PRIVATE ${CMAKE_CURRENT_LIST_DIR}/grib/include
                                        ${CMAKE_CURRENT_LIST_DIR}/grib/src
                                        ${CMAKE_CURRENT_LIST_DIR}/include
                                        ${CMAKE_CURRENT_LIST_DIR}/src
                                        ${PNG_INCLUDE_DIRS}
                                        ${CMAKE_CURRENT_LIST_DIR}/include/sys)

target_link_libraries(Mashroom_lib PRIVATE grib_c ${PNG_LIBRARIES})
target_include_directories(Mashroom_lib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/grib/include
                                        ${CMAKE_CURRENT_LIST_DIR}/grib/src
                                        ${CMAKE_CURRENT_LIST_DIR}/include
                                        ${CMAKE_CURRENT_LIST_DIR}/src
                                        ${PNG_INCLUDE_DIRS}
                                        ${CMAKE_CURRENT_LIST_DIR}/include/sys)

message(STATUS ${PNG_INCLUDE_DIRS})

set_target_properties(Mashroom grib_c Mashroom_lib
                    PROPERTIES
                    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/lib/Debug"
                    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/shared/Debug"
                    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/bin/Debug"
                    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/Release"
                    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/shared/Release"
                    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/bin/Release")