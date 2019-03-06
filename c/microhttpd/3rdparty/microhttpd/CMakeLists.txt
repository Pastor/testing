cmake_minimum_required(VERSION 3.2.1)
project(microhttpd)

set(MICROHTTPD_VERSION "0.9.63")
set(SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/libmicrohttpd-${MICROHTTPD_VERSION}")

set(LIB_SOURCES
        ${SOURCE_DIRECTORY}/src/lib/action_continue.c
        ${SOURCE_DIRECTORY}/src/lib/action_from_response.c
        ${SOURCE_DIRECTORY}/src/lib/action_parse_post.c
        ${SOURCE_DIRECTORY}/src/lib/action_process_upload.c
        ${SOURCE_DIRECTORY}/src/lib/action_suspend.c
        ${SOURCE_DIRECTORY}/src/lib/connection_add.c
        ${SOURCE_DIRECTORY}/src/lib/connection_add.h
        ${SOURCE_DIRECTORY}/src/lib/connection_call_handlers.c
        ${SOURCE_DIRECTORY}/src/lib/connection_call_handlers.h
        ${SOURCE_DIRECTORY}/src/lib/connection_cleanup.c
        ${SOURCE_DIRECTORY}/src/lib/connection_cleanup.h
        ${SOURCE_DIRECTORY}/src/lib/connection_close.c
        ${SOURCE_DIRECTORY}/src/lib/connection_close.h
        ${SOURCE_DIRECTORY}/src/lib/connection_finish_forward.c
        ${SOURCE_DIRECTORY}/src/lib/connection_finish_forward.h
        ${SOURCE_DIRECTORY}/src/lib/connection_info.c
        ${SOURCE_DIRECTORY}/src/lib/connection_options.c
        ${SOURCE_DIRECTORY}/src/lib/connection_update_last_activity.c
        ${SOURCE_DIRECTORY}/src/lib/connection_update_last_activity.h
        ${SOURCE_DIRECTORY}/src/lib/daemon_close_all_connections.c
        ${SOURCE_DIRECTORY}/src/lib/daemon_close_all_connections.h
        ${SOURCE_DIRECTORY}/src/lib/daemon_create.c
        ${SOURCE_DIRECTORY}/src/lib/daemon_destroy.c
        ${SOURCE_DIRECTORY}/src/lib/daemon_epoll.c
        ${SOURCE_DIRECTORY}/src/lib/daemon_epoll.h
        ${SOURCE_DIRECTORY}/src/lib/daemon_get_timeout.c
        ${SOURCE_DIRECTORY}/src/lib/daemon_info.c
        ${SOURCE_DIRECTORY}/src/lib/daemon_ip_limit.c
        ${SOURCE_DIRECTORY}/src/lib/daemon_ip_limit.h
        ${SOURCE_DIRECTORY}/src/lib/daemon_options.c
        ${SOURCE_DIRECTORY}/src/lib/daemon_poll.c
        ${SOURCE_DIRECTORY}/src/lib/daemon_poll.h
        ${SOURCE_DIRECTORY}/src/lib/daemon_quiesce.c
        ${SOURCE_DIRECTORY}/src/lib/daemon_run.c
        ${SOURCE_DIRECTORY}/src/lib/daemon_select.c
        ${SOURCE_DIRECTORY}/src/lib/daemon_select.h
        ${SOURCE_DIRECTORY}/src/lib/daemon_start.c
        ${SOURCE_DIRECTORY}/src/lib/init.c
        ${SOURCE_DIRECTORY}/src/lib/init.h
        ${SOURCE_DIRECTORY}/src/lib/internal.c
        ${SOURCE_DIRECTORY}/src/lib/internal.h
        ${SOURCE_DIRECTORY}/src/lib/Makefile.am
        ${SOURCE_DIRECTORY}/src/lib/Makefile.in
        ${SOURCE_DIRECTORY}/src/lib/memorypool.c
        ${SOURCE_DIRECTORY}/src/lib/memorypool.h
        ${SOURCE_DIRECTORY}/src/lib/mhd_assert.h
        ${SOURCE_DIRECTORY}/src/lib/mhd_byteorder.h
        ${SOURCE_DIRECTORY}/src/lib/mhd_compat.c
        ${SOURCE_DIRECTORY}/src/lib/mhd_compat.h
        ${SOURCE_DIRECTORY}/src/lib/mhd_itc.c
        ${SOURCE_DIRECTORY}/src/lib/mhd_itc.h
        ${SOURCE_DIRECTORY}/src/lib/mhd_itc_types.h
        ${SOURCE_DIRECTORY}/src/lib/mhd_limits.h
        ${SOURCE_DIRECTORY}/src/lib/mhd_locks.h
        ${SOURCE_DIRECTORY}/src/lib/mhd_mono_clock.c
        ${SOURCE_DIRECTORY}/src/lib/mhd_mono_clock.h
        ${SOURCE_DIRECTORY}/src/lib/mhd_sockets.c
        ${SOURCE_DIRECTORY}/src/lib/mhd_sockets.h
        ${SOURCE_DIRECTORY}/src/lib/mhd_str.c
        ${SOURCE_DIRECTORY}/src/lib/mhd_str.h
        ${SOURCE_DIRECTORY}/src/lib/mhd_threads.c
        ${SOURCE_DIRECTORY}/src/lib/mhd_threads.h
        ${SOURCE_DIRECTORY}/src/lib/panic.c
        ${SOURCE_DIRECTORY}/src/lib/reason_phrase.c
        ${SOURCE_DIRECTORY}/src/lib/request.c
        ${SOURCE_DIRECTORY}/src/lib/request_info.c
        ${SOURCE_DIRECTORY}/src/lib/request_resume.c
        ${SOURCE_DIRECTORY}/src/lib/request_resume.h
        ${SOURCE_DIRECTORY}/src/lib/response.c
        ${SOURCE_DIRECTORY}/src/lib/response_for_upgrade.c
        ${SOURCE_DIRECTORY}/src/lib/response_from_buffer.c
        ${SOURCE_DIRECTORY}/src/lib/response_from_callback.c
        ${SOURCE_DIRECTORY}/src/lib/response_from_fd.c
        ${SOURCE_DIRECTORY}/src/lib/response_options.c
        ${SOURCE_DIRECTORY}/src/lib/sysfdsetsize.c
        ${SOURCE_DIRECTORY}/src/lib/sysfdsetsize.h
        ${SOURCE_DIRECTORY}/src/lib/tsearch.c
        ${SOURCE_DIRECTORY}/src/lib/tsearch.h
        ${SOURCE_DIRECTORY}/src/lib/upgrade_process.c
        ${SOURCE_DIRECTORY}/src/lib/upgrade_process.h
        ${SOURCE_DIRECTORY}/src/lib/version.c
        )

set(MICROHTTPD_SOURCES
        ${SOURCE_DIRECTORY}/src/microhttpd/connection.c
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_sockets.c
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_sockets.h
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_threads.c
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_compat.h
        ${SOURCE_DIRECTORY}/src/microhttpd/base64.c
        ${SOURCE_DIRECTORY}/src/microhttpd/md5.h
        ${SOURCE_DIRECTORY}/src/microhttpd/response.h
        ${SOURCE_DIRECTORY}/src/microhttpd/sha256.c
        ${SOURCE_DIRECTORY}/src/microhttpd/memorypool.c
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_mono_clock.h
        ${SOURCE_DIRECTORY}/src/microhttpd/sysfdsetsize.c
        ${SOURCE_DIRECTORY}/src/microhttpd/base64.h
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_str.h
#        ${SOURCE_DIRECTORY}/src/microhttpd/connection_https.h
        ${SOURCE_DIRECTORY}/src/microhttpd/internal.h
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_assert.h
        ${SOURCE_DIRECTORY}/src/microhttpd/postprocessor.c
        ${SOURCE_DIRECTORY}/src/microhttpd/memorypool.h
        ${SOURCE_DIRECTORY}/src/microhttpd/internal.c
        ${SOURCE_DIRECTORY}/src/microhttpd/connection.h
        ${SOURCE_DIRECTORY}/src/microhttpd/sysfdsetsize.h
        ${SOURCE_DIRECTORY}/src/microhttpd/response.c
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_itc.c
#        ${SOURCE_DIRECTORY}/src/microhttpd/connection_https.c
        ${SOURCE_DIRECTORY}/src/microhttpd/basicauth.c
        ${SOURCE_DIRECTORY}/src/microhttpd/md5.c
        ${SOURCE_DIRECTORY}/src/microhttpd/digestauth.c
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_str.c
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_itc_types.h
        ${SOURCE_DIRECTORY}/src/microhttpd/daemon.c
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_mono_clock.c
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_itc.h
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_compat.c
        ${SOURCE_DIRECTORY}/src/microhttpd/sha256.h
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_threads.h
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_locks.h
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_byteorder.h
        ${SOURCE_DIRECTORY}/src/microhttpd/reason_phrase.c
        ${SOURCE_DIRECTORY}/src/microhttpd/mhd_limits.h
        )

include_directories(${SOURCE_DIRECTORY}/src/include)
include_directories(${SOURCE_DIRECTORY}/src/lib)
include_directories(${SOURCE_DIRECTORY}/src/microhttpd)

if (WIN32)
    set(SYSTEM_LIBRARIES ws2_32)
    if (MSVC)
        include_directories(${SOURCE_DIRECTORY}/w32/common)
    else ()
        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mingw)
    endif ()
endif ()

add_library(microhttpd_append STATIC ${LIB_SOURCES})
add_library(microhttpd STATIC ${MICROHTTPD_SOURCES})
add_executable(demo ${SOURCE_DIRECTORY}/src/examples/demo.c)
add_executable(querystring ${SOURCE_DIRECTORY}/src/examples/querystring_example.c)

target_link_libraries(demo microhttpd microhttpd_append ${SYSTEM_LIBRARIES})
target_link_libraries(querystring microhttpd microhttpd_append ${SYSTEM_LIBRARIES})


