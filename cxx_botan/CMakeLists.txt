cmake_minimum_required(VERSION 3.15)
project(local_botan)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(local_botan main.cpp tinyxml2.cpp)
add_library(sqlite3 STATIC sqlite3.c sqlite3.h sqlite3ext.h)
if (WIN32)
    include_directories(windows)
    #python configure.py --with-sqlite3 --amalgamation --single-amalgamation-file --os=mingw --cpu=x86_64 --cc-bin=x86_64-w64-mingw32-g++ --ar-command=x86_64-w64-mingw32-ar
    add_library(botan STATIC windows/botan_all.cpp windows/botan_all.h windows/botan_all_internal.h)
#    target_link_options(botan PUBLIC -Wa,-mbig-obj)
    set_target_properties(botan PROPERTIES COMPILE_FLAGS -Wa,-mbig-obj)
    target_link_libraries(botan PUBLIC sqlite3)
    target_link_libraries(local_botan PUBLIC sqlite3 curl ws2_32)
elseif (UNIX)
    include_directories(unix)
    add_library(botan STATIC unix/botan_all.cpp unix/botan_all.h unix/botan_all_internal.h)
    target_link_options(botan PUBLIC -Wa,-mbig-obj)
    set_target_properties(botan PROPERTIES COMPILE_FLAGS -m64 -Wa,-mbig-obj -O2)
    target_link_libraries(local_botan PUBLIC pthread z sqlite3 dl rt curl)
endif ()