cmake_minimum_required(VERSION 3.4.1)
project(02.Exercise)

add_executable(${PROJECT_NAME}_character_sort_C character_sort.c)
add_executable(${PROJECT_NAME}_file_output_numeric_C file_output_numeric.c)
add_executable(${PROJECT_NAME}_character_sort_CXX character_sort.cxx)
add_executable(${PROJECT_NAME}_file_output_numeric_CXX file_output_numeric.cxx)

add_executable(${PROJECT_NAME}_prime_sieve_eratosthene_CXX prime_sieve_eratosthene.cxx)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/file_output_numeric.txt DESTINATION ${CMAKE_BINARY_DIR})

