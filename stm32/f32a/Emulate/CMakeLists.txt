cmake_minimum_required(VERSION 3.2.1)
project(F32A)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_definitions(-DDEBUG)
include_directories(src)
include_directories(${CURSES_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}_Emulator src/f32.c src/f32_emu.c)

file(COPY test/add.f18 DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY test/message.f18 DESTINATION ${CMAKE_BINARY_DIR}/)
