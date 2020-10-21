//
// Created by kingdo on 10/18/20.
//
#define _GNU_SOURCE

#include <sched.h>
#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include "task/task.h"
#include "tpool.h"

__pid_t task_pid_list__[TASK_MAX_COUNT];
int __task_num = 0;

extern void gogo(void *new_stack, size_t stack_size);

void tpool_create_pool(int pool_size) {
    for (int i = 0; i < pool_size; ++i) {
        CREATE_A_TASK(t)
        /**
         * 这里仅仅设置了CLONE_VM参数而没有设置CLONE_THREAD flag是一个重大的决定，这意味着，我们clone的线程和posix定义的线程不同
         * 可以认为我们定义的是一个共享内存地址空间的进程，这样clone的每个进程依然是一个线程组，但是exec、exit仍然还是一个危险的操作
         *
         * CLONE_SETTLS 使用的也是arch_prctl系统调用，利用FS实现线程存储
         *
         * SIGCHLD，子进程退出时，向父进程发送的signal，如果不发送这个信号，那么是没办法被wait()获取的，只能使用waitpid + __WALL/__WCLONE等参数
         *
        */
        t->tgid = clone(task_birth, t->stack_top,
                        CLONE_VM |
                        CLONE_SETTLS |
                        CLONE_CHILD_SETTID |
                        CLONE_PARENT_SETTID,
                        NULL, &task_pid_list__[i], t, &t->tgid);
        if (t->tgid == -1) {
            printf("clone wrong\n");
        }
//        waitpid(t->tgid, NULL, __WALL);
//        wait(NULL);
//        printf("$$$$$$$$$$$$$$$$\n");
    }
}