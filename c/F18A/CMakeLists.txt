cmake_minimum_required(VERSION 3.2.1)
project(F18A)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Curses REQUIRED)

add_definitions(-DDEBUG)

include_directories(src)
include_directories(${CURSES_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}_Emulator src/f18.c src/f18_emu.c src/socketpair.c)
target_link_libraries(${PROJECT_NAME}_Emulator ${CURSES_LIBRARIES} ws2_32)

file(COPY test/add.f18 DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY test/mult.f18 DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY test/example1.f18 DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY test/example2.f18 DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY test/example3.f18 DESTINATION ${CMAKE_BINARY_DIR}/)