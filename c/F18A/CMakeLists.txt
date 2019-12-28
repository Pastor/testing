cmake_minimum_required(VERSION 3.2.1)
project(F18A)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Curses REQUIRED)

include_directories(src)
include_directories(${CURSES_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}_Emulator src/debugger.c src/emulator.c src/f18a.c src/opcodes.c src/terminal.c)
target_link_libraries(${PROJECT_NAME}_Emulator ${CURSES_LIBRARIES})

file(COPY test/add.f18 DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY test/mult.f18 DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY test/example1.f18 DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY test/example2.f18 DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY test/example3.f18 DESTINATION ${CMAKE_BINARY_DIR}/)