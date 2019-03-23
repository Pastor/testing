cmake_minimum_required(VERSION 3.2.1)
project(3rdparty)

#add_definitions(-DTCC_IS_NATIVE)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_BINARY_DIR})
include_directories(3rdparty/tinycc/src)

if (ARCH STREQUAL "x64")
    add_definitions(-DTCC_TARGET_X86_64)
else()
    add_definitions(-DTCC_TARGET_I386)
endif()

if (WIN32)
    add_definitions(-DTCC_TARGET_PE)
elseif (UNIX)
    add_definitions(-DTCC_TARGET_ELF)
endif ()


add_library(tcc STATIC tinycc/src/libtcc.c)