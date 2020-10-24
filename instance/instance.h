//
// Created by kingdo on 2020/10/23.
//

#ifndef TPFAAS_INSTANCE_H
#define TPFAAS_INSTANCE_H

#include "tool/stack.h"
#include "tool/list_head.h"
#include <string.h>

typedef struct {
    F *f;

    stack_struct stack;

    list_head R_global_wait_queue_list;
    list_head F_local_wait_queue_list;
} I;

#define INIT_I_LIST_HEAD(i) do{  \
    INIT_LIST_HEAD(&i->R_global_wait_queue_list);\
    INIT_LIST_HEAD(&i->R_global_wait_queue_list);\
}while(0);

#endif //TPFAAS_INSTANCE_H
