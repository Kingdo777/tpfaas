//
// Created by kingdo on 10/17/20.
//
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <tool/print.h>
#include <test/test.h>
#include <wait.h>
#include "tool/tls.h"
#include "sync/lock.h"
#include "instance/instance.h"


unsigned long printf_fs;
unsigned long base;

extern __pid_t task_pid_list__[TASK_MAX_COUNT];
extern int task_num__;

stack_struct stack[2];
pthread_mutex_t print_mutex;

int main() {
    GET_PRINTF_FS()
    INIT_LOCK()
    resource res = DEFAULT_RESOURCE;
    func_register(taskF, "taskA", res, 1);
//    func_register(taskF, "taskB", res, 10);

    int count = 100;
    F *f_A = get_func("taskA");
    for (int i = 0; i < count; ++i) {
        make_request(f_A, "hello taskA\n", sizeof("hello taskA\n"));
    }
    printf("done\n");
    sleep(1000000);
    return 0;
}
