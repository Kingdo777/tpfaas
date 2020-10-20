//
// Created by kingdo on 10/20/20.
//

#ifndef TPFAAS_TASK_H
#define TPFAAS_TASK_H

#include "list_head.h"
#include "function.h"
#include <stdlib.h>

#define DEFAULT_STACK_SIZE 8*1024

#define MALLOC_STACK(s_top, s_space, s_size) s_space=malloc(s_size);\
    s_top=s_space+s_size-1;
#define MALLOC_DEFAULT_STACK(s_top, s_space) MALLOC_STACK(s_top, s_space,DEFAULT_STACK_SIZE)

#define CREATE_A_TASK(t) \
    task *t = malloc(sizeof(task)); \
    init_task(t);\
    __task_num++;

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

void init_task(task *t);

#endif //TPFAAS_TASK_H
