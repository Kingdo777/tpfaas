//
// Created by kingdo on 10/17/20.
//

#include <stdbool.h>
#include <memory.h>
#include "function.h"

//记录当前可用的func_id
int __next_func_id = 0;
//记录当前的func_num
int __current_func_num = 0;

func __func_list[MAX_FUNC_NUM];

func_id get_funcId_from_name(const char *function_name) {
    func *f;
    for (int i = 0; i < GET_CURRENT_FUNC_NUM(); ++i) {
        f = &__func_list[i];
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
    func function = {
            .id=funcId,
            .name=function_name,
            .entry_addr=entry_addr,
            .res=DEFAULT_RESOURCE,
    };
    __func_list[funcId] = function;
    CURRENT_FUNC_NUM_INC();
    return true;
}

func *get_func(char *function_name) {
    func_id f_id;
    f_id = get_funcId_from_name(function_name);
    return f_id == -1 ? NULL : &__func_list[f_id];
}
