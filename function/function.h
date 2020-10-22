//
// Created by kingdo on 10/17/20.
//

#ifndef TPFAAS_FUNCTION_H
#define TPFAAS_FUNCTION_H

#include <stdbool.h>
#include "resource.h"

//最大容纳的function数目
#define MAX_FUNC_NUM 1000

//返回当前可用的function的宗数目
#define GET_CURRENT_FUNC_NUM() (__current_func_num)
//__current_func_num自增
#define CURRENT_FUNC_NUM_INC() (++__current_func_num)
//检查并返回下一个可用的func_id，若已经超过了MAX_FUNC_NUM的限制，则返回-1
#define GET_NEXT_FUNC_ID() ((__next_func_id < MAX_FUNC_NUM) ? (__next_func_id++) : -1)
//调用某个function
#define FUN(f, arg) ((void *(*)(void *))(get_func(f)->entry_addr))(arg);


//func_list的索引，用于定位制定的function
typedef int func_id;
//function的结构体
///结构体只能向下加参数，不然gogo会出问题
typedef struct func {
    func_id id;
    const char *name;

    //function的入口地址
    void *(*entry_addr)(void *);

    //资源限制
    resource res;
} func;

bool func_register(void *(*entry_addr)(void *), const char *function_name);

func_id get_funcId_from_name(const char *function_name);

func *get_func(char *function_name);

#endif //TPFAAS_FUNCTION_H
