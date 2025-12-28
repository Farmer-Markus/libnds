cmake_minimum_required(VERSION 4.0)
project(libnds)


option(NDS_STATIC "Build libnds as static library" FALSE)
option(NDS_SHARED "Build libnds as shared library" TRUE)

set(NDS_C_STANDARD 99)

if(NDS_STATIC)
    message(STATUS "Building NDS static library")
endif()

if(NDS_SHARED)
    message(STATUS "Building NDS shared library")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_C_BYTE_ORDER STREQUAL "BIG_ENDIAN")
    message(FATAL_ERROR "Big endian systems are currently not supported!")
    add_compile_definitions(BIG_ENDIAN)
endif()

file(GLOB NDS_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/src/filesystem/*.c ${PROJECT_SOURCE_DIR}/src/utils/*.c)


if(NDS_SHARED)
    add_library(NDS-shared SHARED ${NDS_SOURCES})
    add_library(NDS::NDS ALIAS NDS-shared)
    target_include_directories(NDS-shared
        PUBLIC include
    )
    set_property(TARGET NDS-shared PROPERTY C_STANDARD ${NDS_C_STANDARD})
endif()

if(NDS_STATIC)
    add_library(NDS-static STATIC ${NDS_SOURCES})
    add_library(NDS::NDS-static ALIAS NDS-static)
    target_include_directories(NDS-static
        PUBLIC include
    )
    set_property(TARGET NDS-static PROPERTY C_STANDARD ${NDS_C_STANDARD})
endif()
