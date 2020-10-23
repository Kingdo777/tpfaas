//
// Created by kingdo on 10/17/20.
//

#include <stdbool.h>
#include <memory.h>
#include <stdlib.h>
#include "function.h"
#include "tool/stack.h"

//记录当前可用的func_id
int __next_func_id = 0;
//记录当前的func_num
int __current_func_num = 0;

F func_list__[MAX_FUNC_NUM];

func_id get_funcId_from_name(const char *function_name) {
    F *f;
    for (int i = 0; i < GET_CURRENT_FUNC_NUM(); ++i) {
        f = &func_list__[i];
        if (!strcmp(f->name, function_name))
            return i;
    }
    return -1;
}

bool check_func_name_uniq(const char *function_name) {
    return (get_funcId_from_name(function_name) == -1) ? true : false;
}


bool func_register(void *(*entry_addr)(void *), const char *function_name) {
    func_id funcId;
    if ((funcId = GET_NEXT_FUNC_ID()) == -1)
        return false;
    if (!check_func_name_uniq(function_name))
        return false;
    void *stack_top, *stack_space;
    unsigned long stack_size = DEFAULT_STACK_SIZE;
    MALLOC_DEFAULT_STACK_WITH_TASK_DONE(stack_top, stack_space)
    F function = {
            .id=funcId,
            .name=function_name,
            .entry_addr=entry_addr,
            .res=DEFAULT_RESOURCE,
            .stack.stack_top=stack_top,
            .stack.stack_size=stack_size,
            .stack.stack_space=stack_space
    };
    func_list__[funcId] = function;
    CURRENT_FUNC_NUM_INC();
    return true;
}

F *get_func(char *function_name) {
    func_id f_id;
    f_id = get_funcId_from_name(function_name);
    return f_id == -1 ? NULL : &func_list__[f_id];
}
