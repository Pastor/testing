cmake_minimum_required(VERSION 3.4.1)
project(stm8 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(src)

add_definitions(-DSTM8S103)

set(LIBRARY_SOURCES
        )

set(APP_SOURCES
        src/main.c
        src/circle_buf.c

        src/circle_buf.h)

add_executable(${PROJECT_NAME} ${APP_SOURCES} ${LIBRARY_SOURCES})

