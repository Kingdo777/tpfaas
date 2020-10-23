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

#define CREATE_A_TASK(t)  T *t = creat_task(NULL);
#define CREATE_A_TASK_WITH_FUNC(t, f)  T *t = creat_task(f);

typedef struct T {
    //pid
    __pid_t tgid;
    //futex word
    int futex_word;
    //stack
    stack_struct stack;
    //next_function
    F *next_func;

    //当前正在为那个F工作
    F *work_for;

    //一个全局的task链表，目前来说在RFIT模型中没有什么用处
    list_head task_list;
    //链接在R上的空闲链表
    list_head task_idle_list;
    //链接在R上的工作链表
    list_head task_busy_list;
    //链接在F上的同时为该F工作的task
    list_head task_func_list;
} T;

int task_birth(void *arg);

T *creat_task(F *f);

void init_task(T *t, F *f);

void gogo(T *t);

#endif //TPFAAS_TASK_H
