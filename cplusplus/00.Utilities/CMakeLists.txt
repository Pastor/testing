cmake_minimum_required(VERSION 3.4.1)
project(00.Utilities)

add_executable(CourseCalculator course_calculator.cpp)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/students.txt DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

