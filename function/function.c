//
// Created by kingdo on 10/17/20.
//

#include <stdbool.h>
#include <memory.h>
#include <stdlib.h>
#include "function.h"
#include "tool/stack.h"
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


bool func_register(void *(*entry_addr)(void *), const char *function_name, resource res, int concurrent_count) {
    if (!check_func_name_uniq(function_name))
        return false;
    F *f = (F *) malloc(sizeof(F));
    INIT_F_LIST_HEAD(f)
    f->name = function_name;
    f->res = get_res(res);
    f->entry_addr = entry_addr;
    f->concurrent_enable = concurrent_count > 1;
    f->queue_list_max_cap = f->concurrent_enable ? concurrent_count : 1;
    f->all_queue_list_rest_cap = 0;
    list_add(&f->func_list, &func_list_head);
//    func_id funcId;
//    if ((funcId = GET_NEXT_FUNC_ID()) == -1)
//        return false;
//    if (!check_func_name_uniq(function_name))
//        return false;
//    void *stack_top, *stack_space;
//    unsigned long stack_size = DEFAULT_STACK_SIZE;
//    MALLOC_DEFAULT_STACK_WITH_TASK_DONE(stack_top, stack_space)
//    F function = {
//            .id=funcId,
//            .name=function_name,
//            .entry_addr=entry_addr,
//            .res=DEFAULT_RESOURCE,
//            .stack.stack_top=stack_top,
//            .stack.stack_size=stack_size,
//            .stack.stack_space=stack_space
//    };
//    func_list__[funcId] = function;
//    CURRENT_FUNC_NUM_INC();
    return true;
}

F *get_func(const char *function_name) {
    return get_func_from_name(function_name);
}
