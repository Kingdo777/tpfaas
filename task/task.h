//
// Created by kingdo on 10/20/20.
//

#ifndef TPFAAS_TASK_H
#define TPFAAS_TASK_H

#include "tool/list_head.h"
#include "function/function.h"
#include "tool/stack.h"
#include <stdlib.h>
#include <tool/queue.h>
#include "instance/instance.h"

extern void gogo_switch_new_free_old(void *new_stack, void *old_stack);

extern void gogo_switch_new_free_old_and_jmp(void *new_stack, void *old_stack, void *new_pc);

extern list_head res_list_head;

#define TRY_COUNT 3

#define TASK_MAX_COUNT 10000

typedef struct T {
    //pid
    __pid_t tgid;
    //futex word
    int futex_word;
    //stack
    stack_struct stack;

    //当前正在为那个F工作
    F *work_for;

    //正在处理Instance
    I *deal_with;

    //针对T正在处理不能并发的函数，此时T是直接被指定一个I的，而不需要自己寻找，此标志位帮助get_instance区分
    bool direct_run;

    //本地队列的长度
    pthread_mutex_t T_local_I_queue_lock;
    T_local_I_list *i_queue;

    //一个全局的task链表，目前来说在RFIT模型中没有什么用处
    list_head task_list;
    //链接在R上的空闲链表
    list_head task_idle_list;
    //链接在R上的工作链表
    list_head task_busy_list;
    //链接在F上的同时为该F工作的task
    list_head task_func_list;
} T;
#define INIT_T_LIST_HEAD(t) do{  \
    INIT_LIST_HEAD(&t->task_list);\
    INIT_LIST_HEAD(&t->task_idle_list);\
    INIT_LIST_HEAD(&t->task_busy_list);\
    INIT_LIST_HEAD(&t->task_func_list);\
}while(0);

//int task_birth(void *arg);

T *creat_T(I *i);

bool init_task(T *t, I *i);

bool bind_os_thread(T *t);

void gogo(T *t);

bool bind_os_thread_(T *t);

void release_err_task(T *t);

void release_task_stack_when_sleep(T *t);

bool malloc_task_stack_when_create(T *t);

T *get_T_for_I(I *i);

void put_T_into_R_idle_task_list(T *t, R *r);

void remove_T_from_R_idle_task_list(T *t, R *r);

void put_T_into_R_busy_task_list(T *t, R *r);

void remove_T_from_R_busy_task_list(T *t, R *r);

#endif //TPFAAS_TASK_H
