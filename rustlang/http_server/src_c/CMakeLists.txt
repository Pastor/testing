cmake_minimum_required(VERSION 3.2.1)
project(sqlite3)

set(CMAKE_C_STANDARD 11)

add_library(${PROJECT_NAME} STATIC sqlite3.c)
