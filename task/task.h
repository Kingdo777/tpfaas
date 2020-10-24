//
// Created by kingdo on 10/20/20.
//

#ifndef TPFAAS_TASK_H
#define TPFAAS_TASK_H

#include "tool/list_head.h"
#include "function/function.h"
#include "tool/stack.h"
#include <stdlib.h>
#include "instance/instance.h"

extern void gogo_switch(void *new_stack);

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

    //本地队列的长度
    pthread_mutex_t F_local_wait_queue_lock;
    int F_local_wait_queue_size;
    //表头,本地的I队列
    list_head F_local_wait_queue_head;

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
    INIT_LIST_HEAD(&t->F_local_wait_queue_head);\
}while(0);

//int task_birth(void *arg);

T *creat_new_task(R *r);

bool init_task(T *t);

bool bind_os_thread(T *t);

void gogo(T *t);

bool bind_os_thread_(T *t);

void release_err_task(T *t);

void release_task_stack_when_sleep(T *t);

bool malloc_task_stack_when_create(T *t);

#endif //TPFAAS_TASK_H
