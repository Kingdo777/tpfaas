//
// Created by kingdo on 10/18/20.
//
#include <unistd.h>
#include "task/task.h"
#include "tpool.h"

__pid_t task_pid_list__[TASK_MAX_COUNT];
int task_num__ = 0;

void tpool_create_pool(int pool_size) {
    for (int i = 0; i < pool_size; ++i) {
        add_task_2_pool();
    }
}

void add_task_2_pool() {
    CREATE_A_TASK(t)
}

void add_task_2_pool_with_func(func *f) {
    CREATE_A_TASK_WITH_FUNC(t, f)
}