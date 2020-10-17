cmake_minimum_required(VERSION 3.12)
project(tpfaas C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

include_directories(.)

add_executable(tpfaas
        main.c)
