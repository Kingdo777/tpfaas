//
// Created by kingdo on 10/17/20.
//

#ifndef TPFAAS_FUNCTION_H
#define TPFAAS_FUNCTION_H

#include <stdbool.h>
#include "resource/resource.h"

//调用某个function
#define FUN(f, arg) ((void *(*)(void *))(get_func(f)->entry_addr))(arg);


///结构体只能向下加参数，不然gogo会出问题
typedef struct {
    const char *name;

    //function的入口地址
    void *(*entry_addr)(void *);

    //资源限制
    R *r;

    bool concurrent_enable;
    //一个极其重要的对象，表示了F可以在一个T上的并发数目执行I的数目，同时也是单个I队列的长度
    int concurrent_count;

    //访问下面任何一个变量都需要加相同的锁
    pthread_mutex_t F_global_wait_i_lock;
    //又是一个及其重要的参数，表示了F的剩余可处理容量=(T*C-Running)
    int F_global_wait_i_list_rest_cap;
    //当前全局队列中等待的I个数
    int wait_I_count;
    //正在work_for此F的T的个数
    int work_T_count;
    //链表头 指向了instance队列
    list_head F_global_wait_i_head;

    //func的全局链表，之前用的是数组，好像在RFIT模型中也没什么卵用
    list_head func_list_list;
    //链接到R上的func链表
    list_head resource_func_list;
} F;

#define INIT_F_LIST_HEAD(f) do{  \
    INIT_LIST_HEAD(&f->func_list_list);\
    INIT_LIST_HEAD(&f->resource_func_list); \
    INIT_LIST_HEAD(&f->F_global_wait_i_head); \
}while(0);

bool func_register(void *(*entry_addr)(void *), const char *function_name, resource res, int concurrent_count);

F *get_func_from_name(const char *function_name);

F *get_func(const char *function_name);

void create_global_I_queue(F *f, bool is_wait);

void create_global_I_wait_queue(F *f);

#endif //TPFAAS_FUNCTION_H
