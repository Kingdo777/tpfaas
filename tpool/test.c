//
// Created by kingdo on 2020/11/3.
//
#include <stdio.h>
#include <zconf.h>
#include "tpool.h"

void *fun(void *args) {
    int thread = (int) args;
    printf("running the thread of %d\n", thread);
    return NULL;
}

int main_tpool(int argc, char *args[]) {
    tpool_t *pool = NULL;
    if (0 != create_tpool(&pool, 5000)) {
        printf("create_tpool failed!\n");
        return -1;
    }

    for (int i = 0; i < 500; i++) {
        add_task_2_tpool(pool, fun, (void *) i);
    }
    sleep(2);
    destroy_tpool(pool);
    return 0;
}