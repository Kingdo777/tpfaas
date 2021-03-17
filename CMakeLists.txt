cmake_minimum_required(VERSION 3.12)
project(tpfaas C ASM)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

add_executable(tpfaas
        src/main.c
        src/invoke.c
        src/function/function.c
        src/sync/sync.c include/sync.h
        src/tool/list_head.c
        src/task/task.c
        src/resource/resource.c
        src/instance/instance.c
        include/test.h src/test/test.c
        src/tool/debug.c)

target_link_libraries(${PROJECT_NAME} -lfcgi -lpthread)
