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


LIST_HEAD(func_list_head)

F *get_func_from_name(const char *function_name) {
    F *f;
    list_for_each_entry(f, &func_list_head, func_list_list) {
        if (!strcmp(f->name, function_name))
            return f;
    }
    return NULL;
}

bool check_func_name_uniq(const char *function_name) {
    return (get_func_from_name(function_name) == NULL) ? true : false;
}

void create_global_I_queue(F *f, bool is_wait) {
    F_global_I_queue *queue = malloc(sizeof(F_global_I_queue));
    queue->is_wait_queue = is_wait;
    INIT_LIST_HEAD(&queue->F_global_I_queue_list);
    queue->i_queue = malloc(sizeof(Instance_queue));
    queue->i_queue->queue_list_max_cap = is_wait ? f->concurrent_count : INT_MAX;
    queue->i_queue->queue_list_size = 0;
    INIT_LIST_HEAD(&queue->i_queue->instance_list_head);
    //run_queue被放在链表的最后面
    if (is_wait) {
        list_add(&f->F_global_I_queue_head, &queue->F_global_I_queue_list);
    } else {
        //run_queue让放在队尾巴，这样倒序查找的第一个就是run_queue
        list_add_tail(&f->F_global_I_queue_head, &queue->F_global_I_queue_list);
    }
}

void create_global_I_wait_queue(F *f) {
    create_global_I_queue(f, true);
}

void create_global_I_run_queue(F *f) {
    create_global_I_queue(f, false);
}

void init_func(F *f, void *(*entry_addr)(void *), const char *function_name, resource res, int concurrent_count) {
    INIT_F_LIST_HEAD(f)
    f->name = function_name;
    f->r = get_res(res);
    f->entry_addr = entry_addr;
    f->concurrent_enable = concurrent_count > 1;
    f->concurrent_count = f->concurrent_enable ? concurrent_count : 1;
    //无比重要的对象
    f->all_global_I_queue_rest_cap = 0;
    f->wait_I_count = 0;
    if (f->concurrent_enable) {
        create_global_I_run_queue(f);
        pthread_mutex_init(&f->F_global_I_queue_lock, NULL);
    }

}

bool func_register(void *(*entry_addr)(void *), const char *function_name, resource res, int concurrent_count) {
    if (!check_func_name_uniq(function_name))
        return false;
    F *f = (F *) malloc(sizeof(F));
    init_func(f, entry_addr, function_name, res, concurrent_count);
    list_add(&f->r->resource_func_head, &f->resource_func_list);
    list_add(&f->func_list_list, &func_list_head);
    return true;
}

F *get_func(const char *function_name) {
    return get_func_from_name(function_name);
}
