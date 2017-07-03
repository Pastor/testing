cmake_minimum_required(VERSION 3.4.1)
project(3rdparty)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(sqlite3)
include_directories(mongoose)

set(SQLITE3_FILES  sqlite3/sqlite3.c)
set(MONGOOSE_FILES mongoose/mongoose.c)
add_definitions(
        -DMG_ENABLE_HTTP_STREAMING_MULTIPART
        -DMG_ENABLE_IPV6
        -DMG_ENABLE_BROADCAST
        -DMG_ENABLE_GETADDRINFO
        -DMG_ENABLE_THREADS
        -DMG_DISABLE_MD5
        -DMG_DISABLE_HTTP_DIGEST_AUTH
        -DMG_VPRINTF_BUFFER_SIZE=1024
)

add_subdirectory(gtest/googletest)

add_library(sqlite3  STATIC ${SQLITE3_FILES})
add_library(mongoose STATIC ${MONGOOSE_FILES})