cmake_minimum_required(VERSION 3.10)
project(SKIP)

add_library(skip SHARED skip.c)
add_executable(main main.cpp)
target_link_libraries(main skip)
