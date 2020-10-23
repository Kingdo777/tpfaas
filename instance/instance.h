//
// Created by kingdo on 2020/10/23.
//

#ifndef TPFAAS_INSTANCE_H
#define TPFAAS_INSTANCE_H

#include "tool/stack.h"
#include "tool/list_head.h"

typedef struct  {
    stack_struct stack;

    list_head R_global_wait_queue_list;
    list_head F_local_wait_queue_list;
} I;
#endif //TPFAAS_INSTANCE_H
