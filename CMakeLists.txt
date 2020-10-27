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
        sync/sync.c sync/sync.h sync/lock.h
        tool/list_head.h tool/tls.c tool/tls.h tool/print.h tool/stack.h tool/queue.h tool/list_head.c
        task/task.c task/task.h task/gogo.c task/gogo.s
        resource/resource.c
        instance/instance.c instance/instance.h
        doc/RFIT-model.h test/test.h)
