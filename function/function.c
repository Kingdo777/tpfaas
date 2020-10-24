//
// Created by kingdo on 10/17/20.
//

#include <stdbool.h>
#include <memory.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <instance/instance.h>
#include "function.h"
#include "tool/stack.h"
#include "tool/queue.h"
#include "tool/list_head.h"
#include "resource/resource.h"

//该用链表结构，下面的变量全部抛弃
//记录当前可用的func_id
//int __next_func_id = 0;
//记录当前的func_num
//int __current_func_num = 0;
//F func_list__[MAX_FUNC_NUM];

LIST_HEAD(func_list_head)

F *get_func_from_name(const char *function_name) {
    F *f;
    list_for_each_entry(f, &func_list_head, func_list) {
        if (!strcmp(f->name, function_name))
            return f;
    }
    return NULL;
}

bool check_func_name_uniq(const char *function_name) {
    return (get_func_from_name(function_name) == NULL) ? true : false;
}

void create_global_I_wait_queue_and_add_I_to_it(F *f, I *i) {
    F_global_I_queue *run_queue = malloc(sizeof(F_global_I_queue));
    run_queue->is_wait_queue = true;
    INIT_LIST_HEAD(&run_queue->F_global_I_queue_list);
    run_queue->i_queue = malloc(sizeof(instance_queue));
    //run_queue的最大不不设置限制的
    run_queue->i_queue->queue_list_max_cap = f->concurrent_count;
    //添加了新的I
    run_queue->i_queue->queue_list_size = 1;
    INIT_LIST_HEAD(&run_queue->i_queue->instance_list_head);
    list_add(&f->F_global_I_queue_head, &run_queue->F_global_I_queue_list);
    list_add(&i->F_global_wait_queue_list, &f->F_global_I_queue_head);

}

void create_global_I_run_queue(F *f) {
    F_global_I_queue *run_queue = malloc(sizeof(F_global_I_queue));
    run_queue->is_wait_queue = false;
    INIT_LIST_HEAD(&run_queue->F_global_I_queue_list);
    run_queue->i_queue = malloc(sizeof(instance_queue));
    //run_queue的最大不不设置限制的
    run_queue->i_queue->queue_list_max_cap = INT_MAX;
    run_queue->i_queue->queue_list_size = 0;
    INIT_LIST_HEAD(&run_queue->i_queue->instance_list_head);
    //run_queue被放在链表的最后面
    list_add(&f->F_global_I_queue_head, &run_queue->F_global_I_queue_list);
}

void init_func(F *f, void *(*entry_addr)(void *), const char *function_name, resource res, int concurrent_count) {
    INIT_F_LIST_HEAD(f)
    f->name = function_name;
    f->res = get_res(res);
    f->entry_addr = entry_addr;
    f->concurrent_enable = concurrent_count > 1;
    f->concurrent_count = f->concurrent_enable ? concurrent_count : 1;
    //无比重要的对象
    f->all_global_I_queue_rest_cap = 0;
    f->wait_I_count = 0;
    if (f->concurrent_enable) {
        create_global_I_run_queue(f);
    } else {
        //TODO 如果是1,那么没有如此复杂的流程
    }
    pthread_mutex_init(&f->F_global_I_queue_lock, NULL);
    pthread_mutex_init(&f->func_task_lock, NULL);
}

bool func_register(void *(*entry_addr)(void *), const char *function_name, resource res, int concurrent_count) {
    if (!check_func_name_uniq(function_name))
        return false;
    F *f = (F *) malloc(sizeof(F));
    init_func(f, entry_addr, function_name, res, concurrent_count);
    //TODO 下面的操作目前来看暂时是不需要加锁的
    list_add(&f->res->function_head, &f->func_list);
    list_add(&f->func_list, &func_list_head);
    return true;
}

F *get_func(const char *function_name) {
    return get_func_from_name(function_name);
}
