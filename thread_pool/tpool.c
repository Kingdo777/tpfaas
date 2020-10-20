//
// Created by kingdo on 10/18/20.
//
#define _GNU_SOURCE

#include <sched.h>
#include <zconf.h>
#include <stdio.h>
#include <syscall.h>
#include <tool/tls.h>
#include <stdlib.h>
#include "tpool.h"
#include "sync/sync.h"

__pid_t __task_pid_list[TASK_MAX_COUNT];
int __task_num = 0;

LIST_HEAD(__task_list);

int init_task(void *arg) {
    task *t;
    get_tls(&t);
    if (t->futex_word == DEFAULT_FUTEX_WORD) {
        puts("child thread get tls true\n");
    }
    for (int i = 0; i < 5; i++) {
        puts("child ready to work\n");
        sleep(1);
    }
    thread_sleep(&t->futex_word);
    for (int i = 0; i < 5; i++) {
        puts("child back to work\n");
        sleep(1);
    }
    sleep(10000);
}

void tpool_create_pool(int pool_size) {
    for (int i = 0; i < pool_size; ++i) {
        task *t = malloc(sizeof(task));
        t->futex_word = DEFAULT_FUTEX_WORD;
        INIT_LIST_HEAD(&t->task_list_node);
        list_add(&t->task_list_node, &__task_list);
        /**
         * 这里仅仅设置了CLONE_VM参数而没有设置CLONE_THREAD flag是一个重大的决定，这意味着，我们clone的线程和posix定义的线程不同
         * 可以认为我们定义的是一个共享内存地址空间的进程，这样clone的每个进程依然是一个线程组，但是exec仍然还是一个危险的操作
        */
        t->tgid = clone(init_task, MALLOC_DEFAULT_STACK(),
                        CLONE_VM | CLONE_SETTLS | CLONE_CHILD_SETTID | CLONE_PARENT_SETTID,
                        NULL, &__task_pid_list[i], t, &t->tgid);
        __task_num++;

    }
}