cmake_minimum_required(VERSION 3.12)
project(tpfaas C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

include_directories(.)

add_executable(tpfaas
        main.c
        invoke.c invoke.h
        function/function.c function/function.h
        resource/resource.h
        thread_pool/tpool.c thread_pool/tpool.h
        sync/sync.c sync/sync.h
        tool/list_head.h tool/tls.c tool/tls.h
        task/task.c task/task.h task/gogo.s tool/print.h tool/stack.h task/gogo.c resource/resource.c instance/instance.c instance/instance.h tool/queue.h doc/RFIT-model.h tool/list_head.c)
