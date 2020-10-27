//
// Created by kingdo on 2020/10/23.
//

#ifndef TPFAAS_INSTANCE_H
#define TPFAAS_INSTANCE_H

#include "tool/stack.h"
#include "tool/list_head.h"
#include "function/function.h"
#include <string.h>

typedef struct {
    F *f;

    stack_struct stack;

    list_head F_global_wait_queue_list;
    list_head I_local_wait_queue_list;
} I;

#define INIT_I_LIST_HEAD(i) do{  \
    INIT_LIST_HEAD(&i->F_global_wait_queue_list);\
    INIT_LIST_HEAD(&i->I_local_wait_queue_list);\
}while(0);

void release_instance_space(I *i);

void make_request(F *f, void *agr, size_t arg_size);

bool malloc_instance_stack_when_create(I *i, void *arg, size_t arg_size);


#endif //TPFAAS_INSTANCE_H
