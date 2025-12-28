cmake_minimum_required(VERSION 4.0)
project(libnds)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)

if(CMAKE_C_BYTE_ORDER STREQUAL "BIG_ENDIAN")
    message(FATAL_ERROR "Big endian systems are currently not supported!")
    add_compile_definitions(BIG_ENDIAN)
endif()

file(GLOB NDS_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/src/filesystem/*.c ${PROJECT_SOURCE_DIR}/src/utils/*.c)

#add_library(nds SHARED ${NDS_SOURCES})
add_executable(nds ${NDS_SOURCES})
target_include_directories(nds
    PUBLIC include
)