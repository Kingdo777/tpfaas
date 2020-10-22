//
// Created by kingdo on 10/20/20.
//

#ifndef TPFAAS_TASK_H
#define TPFAAS_TASK_H

#include "tool/list_head.h"
#include "function/function.h"
#include "thread_pool/tpool.h"
#include <stdlib.h>

extern int task_num__;
extern pid_t task_pid_list__[TASK_MAX_COUNT];

#define DEFAULT_STACK_SIZE 8*1024

#define MALLOC_STACK_SPACE(s_size) malloc(s_size)
#define MALLOC_DEFAULT_STACK_SPACE() MALLOC_STACK_SPACE(DEFAULT_STACK_SIZE)
#define MALLOC_STACK(s_top, s_space, s_size) s_space=malloc(s_size);\
    s_top=s_space+s_size;
#define MALLOC_DEFAULT_STACK(s_top, s_space) MALLOC_STACK(s_top, s_space,DEFAULT_STACK_SIZE)

#define CREATE_A_TASK(t)  task *t = creat_task(NULL);
#define CREATE_A_TASK_WITH_FUNC(t, f)  task *t = creat_task(f);

typedef struct task {
    //pid
    __pid_t tgid;
    //futex word
    int futex_word;
    //stack
    void *stack_space;//用于栈空间的手动释放
    void *stack_top;
    unsigned long stack_size;
    //next_function
    func *next_func;
    list_head task_list_node;
} task;

int task_birth(void *arg);

void task_done();

task *creat_task(func *f);

void init_task(task *t, func *f);

#endif //TPFAAS_TASK_H
