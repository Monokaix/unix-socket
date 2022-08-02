cmake_minimum_required(VERSION 3.19)
project(epoll_demo C)

set(CMAKE_C_STANDARD 11)

add_executable(epoll_demo_server server.c)

add_executable(epoll_demo_client client.c)