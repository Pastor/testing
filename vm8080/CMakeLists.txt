cmake_minimum_required(VERSION 3.9)
project(vm8080)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.build_w64")

find_package(SDL2 REQUIRED)

add_executable(vm8080 src/vm8080.c src/vm8080.h src/main.c)
target_link_libraries(vm8080 -lSDL2 -lSDL2_image -lSDL2main)

file(COPY ${CMAKE_SOURCE_DIR}/game/invaders.e DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY ${CMAKE_SOURCE_DIR}/game/invaders.f DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY ${CMAKE_SOURCE_DIR}/game/invaders.g DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY ${CMAKE_SOURCE_DIR}/game/invaders.h DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

