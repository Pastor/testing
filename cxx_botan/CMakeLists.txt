cmake_minimum_required(VERSION 3.15)
project(local_botan)

set(CMAKE_CXX_STANDARD 14)

add_executable(local_botan main.cpp tinyxml2.cpp)
add_library(botan STATIC botan_all.cpp botan_all.h botan_all_internal.h )
add_library(sqlite3 STATIC sqlite3.c sqlite3.h sqlite3ext.h )
target_link_libraries(local_botan PUBLIC pthread z sqlite3 dl rt curl botan)