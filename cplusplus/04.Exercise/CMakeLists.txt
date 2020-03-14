cmake_minimum_required(VERSION 3.4.1)
project(04.Exercise)

add_executable(${PROJECT_NAME}_replace_space_CXX file_replace_space.cxx)
add_executable(${PROJECT_NAME}_replace_ws_to_nl_CXX file_replace_ws_to_nl.cxx)
add_executable(${PROJECT_NAME}_intel_hex_format_C intel_hex_format.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/example.hex DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/example_eof.hex DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/example_single.hex DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/marlin_direct_dual.hex DESTINATION ${CMAKE_BINARY_DIR})
