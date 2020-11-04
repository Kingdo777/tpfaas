//
// Created by kingdo on 10/17/20.
//

#include <stdbool.h>
#include <memory.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include "function.h"
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


void init_func(F *f, void *(*entry_addr)(void *), const char *function_name, resource res, int concurrent_count) {
    INIT_F_LIST_HEAD(f)
    f->name = function_name;
    f->r = get_res(res);
    f->entry_addr = entry_addr;
    f->concurrent_enable = concurrent_count > 1;
    f->concurrent_count = f->concurrent_enable ? concurrent_count : 1;
    if (f->concurrent_enable) {
        //无比重要的对象
        f->F_global_wait_i_list_rest_cap = 0;
        f->wait_I_count = 0;
        f->work_T_count = 0;
        pthread_mutex_init(&f->F_global_wait_i_lock, NULL);
    }

}

bool func_register(void *(*entry_addr)(void *), const char *function_name, resource res, int concurrent_count) {
    if (!check_func_name_uniq(function_name))
        return false;
    F *f = (F *) malloc(sizeof(F));
    init_func(f, entry_addr, function_name, res, concurrent_count);
    list_add(&f->resource_func_list, &f->r->resource_func_head);
    list_add(&f->func_list_list, &func_list_head);
    return true;
}

F *get_func(const char *function_name) {
    return get_func_from_name(function_name);
}
