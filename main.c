//
// Created by kingdo on 10/17/20.
//
#define _GNU_SOURCE

#include <sched.h>
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

void gogo_(stack_struct *stack) {
    get_tls(&stack);
    void *old_stack_space = stack[1].stack_space;

    stack_struct stack_new = stack[0];
    PRINTF("%s\n", stack_new.stack_top)

    gogo_switch_new_free_old_and_jmp(stack_new.stack_top, old_stack_space, NULL);
}

void task_done_() {
    stack_struct *stack;
    get_tls(&stack);
    gogo_(stack);
}

stack_struct stack[2];

int main() {
//    GET_PRINTF_FS()
//    INIT_LOCK()
//
//    MALLOC_DEFAULT_STACK(stack[0].stack_top, stack[0].stack_space)
//    stack[0].stack_size = DEFAULT_STACK_SIZE;
//
//    MALLOC_DEFAULT_STACK(stack[1].stack_top, stack[1].stack_space);
//    COPY_STACK_DATA(stack[1].stack_top, "HelloA\n", sizeof("HelloA\n"))
//    stack[1].stack_size = DEFAULT_STACK_SIZE;
//
//    pid_t pid = clone((void *) gogo_, stack[1].stack_top, CLONE_VM | CLONE_SETTLS,
//                      NULL, NULL, stack, NULL);
//    waitpid(pid, NULL, __WALL);
//    return 0;
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
