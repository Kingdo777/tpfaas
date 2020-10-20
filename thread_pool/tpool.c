//
// Created by kingdo on 10/18/20.
//
#define _GNU_SOURCE

#include <sched.h>
#include <zconf.h>
#include <stdio.h>
#include <syscall.h>
#include "tool/tls.h"
#include "task/task.h"
#include "tpool.h"
#include "sync/sync.h"

__pid_t __task_pid_list[TASK_MAX_COUNT];
int __task_num = 0;

extern void gogo(void *new_stack, size_t stack_size);

void tpool_create_pool(int pool_size) {
    for (int i = 0; i < pool_size; ++i) {
        CREATE_A_TASK(t)
        /**
         * 这里仅仅设置了CLONE_VM参数而没有设置CLONE_THREAD flag是一个重大的决定，这意味着，我们clone的线程和posix定义的线程不同
         * 可以认为我们定义的是一个共享内存地址空间的进程，这样clone的每个进程依然是一个线程组，但是exec仍然还是一个危险的操作
        */
        t->tgid = clone(task_birth, t->stack_top,
                        CLONE_VM | CLONE_SETTLS | CLONE_CHILD_SETTID | CLONE_PARENT_SETTID,
                        NULL, &__task_pid_list[i], t, &t->tgid);

    }
}