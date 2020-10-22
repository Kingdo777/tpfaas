//
// Created by kingdo on 10/20/20.
//

#ifndef TPFAAS_TASK_H
#define TPFAAS_TASK_H

#include "tool/list_head.h"
#include "function/function.h"
#include "thread_pool/tpool.h"
#include "tool/stack.h"
#include <stdlib.h>

extern int task_num__;
extern pid_t task_pid_list__[TASK_MAX_COUNT];

#define CREATE_A_TASK(t)  task *t = creat_task(NULL);
#define CREATE_A_TASK_WITH_FUNC(t, f)  task *t = creat_task(f);

typedef struct task {
    //pid
    __pid_t tgid;
    //futex word
    int futex_word;
    //stack
    stack_struct stack;
    //next_function
    func *next_func;
    list_head task_list_node;
} task;

int task_birth(void *arg);

void task_done();

task *creat_task(func *f);

void init_task(task *t, func *f);

void gogo(task *t);

#endif //TPFAAS_TASK_H
