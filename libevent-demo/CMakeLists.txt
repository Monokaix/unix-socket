cmake_minimum_required(VERSION 3.22)
project(libevent_demo C)
set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(libevent_demo main.c )

target_link_libraries(libevent_demo ${PROJECT_SOURCE_DIR}/lib/libevent.a
        ${PROJECT_SOURCE_DIR}/lib/libevent_core.a
        ${PROJECT_SOURCE_DIR}/lib/libevent_extra.a
        ${PROJECT_SOURCE_DIR}/lib/libevent_openssl.a
        ws2_32.lib
        iphlpapi.lib)
