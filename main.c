//
// Created by kingdo on 10/17/20.
//
#define _GNU_SOURCE

#include <sched.h>
#include <unistd.h>
#include <tool/print.h>
#include <test/test.h>
#include <wait.h>
#include <malloc.h>
#include "tool/tls.h"
#include "sync/lock.h"
#include "instance/instance.h"


unsigned long printf_fs;
unsigned long base;

extern __pid_t task_pid_list__[TASK_MAX_COUNT];
extern int task_num__;

stack_struct stack[2];

int main() {
    GET_PRINTF_FS()
    INIT_LOCK()
    resource res = DEFAULT_RESOURCE;
    func_register(taskF, "taskA", res, 1);
//    func_register(taskF, "taskB", res, 10);
    F *f_A = get_func("taskA");
//    F *f_B = get_func("taskB");
    make_request(f_A, "abcdefg", sizeof("abcdefg"));
//    make_request(f_B, "Hello B\n", sizeof("Hello A\n"));

    waitpid(task_pid_list__[task_num__ - 1], NULL, __WALL);
    return 0;
}
