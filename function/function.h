//
// Created by kingdo on 10/17/20.
//

#ifndef TPFAAS_FUNCTION_H
#define TPFAAS_FUNCTION_H

#include <stdbool.h>
#include "resource/resource.h"
#include "tool/stack.h"

//最大容纳的function数目
//#define MAX_FUNC_NUM 1000
//返回当前可用的function的宗数目
//#define GET_CURRENT_FUNC_NUM() (__current_func_num)
//__current_func_num自增
//#define CURRENT_FUNC_NUM_INC() (++__current_func_num)
//检查并返回下一个可用的func_id，若已经超过了MAX_FUNC_NUM的限制，则返回-1
//#define GET_NEXT_FUNC_ID() ((__next_func_id < MAX_FUNC_NUM) ? (__next_func_id++) : -1)
//调用某个function
#define FUN(f, arg) ((void *(*)(void *))(get_func(f)->entry_addr))(arg);


//func_list的索引，用于定位制定的function
//typedef int func_id;
//function的结构体
///结构体只能向下加参数，不然gogo会出问题
typedef struct {
//    func_id id;
    const char *name;

    //function的入口地址
    void *(*entry_addr)(void *);

    //资源限制
    R *r;

    bool concurrent_enable;
    //一个极其重要的对象，表示了F可以在一个T上的并发数目执行I的数目，同时也是单个I队列的长度
    int concurrent_count;

    //访问下面任何一个变量都需要加相同的锁
    pthread_mutex_t F_global_I_queue_lock;
    //又是一个及其重要的参数，表示了当前正在处理F的剩余容量
    int all_global_I_queue_rest_cap;
    //当前队列中等待I的个数
    int wait_I_count;
    //链表头 指向了instance队列
    list_head F_global_I_queue_head;

    //好像没什么乱用啊，还增复杂度
//    pthread_mutex_t func_task_lock;
//    //链表头 指向了为此F服务的所有的T
//    list_head func_task_head;

    //func的全局链表，之前用的是数组，好像在RFIT模型中也没什么卵用
    list_head func_list;
    //链接到R上的func链表
    list_head resource_func_list;
} F;

#define INIT_F_LIST_HEAD(f) do{  \
    INIT_LIST_HEAD(&f->func_list);\
    INIT_LIST_HEAD(&f->resource_func_list); \
    INIT_LIST_HEAD(&f->F_global_I_queue_head); \
}while(0);

bool func_register(void *(*entry_addr)(void *), const char *function_name, resource res, int concurrent_count);

F *get_func_from_name(const char *function_name);

//func_id get_funcId_from_name(const char *function_name);
F *get_func(const char *function_name);

void create_global_I_queue(F *f, bool is_wait);

void create_global_I_wait_queue(F *f);

#endif //TPFAAS_FUNCTION_H
