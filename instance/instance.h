//
// Created by kingdo on 2020/10/23.
//

#ifndef TPFAAS_INSTANCE_H
#define TPFAAS_INSTANCE_H

#include "tool/list_head.h"
#include "function/function.h"
#include <string.h>
#include <ucontext.h>
#include <fcgiapp.h>

#define STACK_DEFAULT_SIZE (8*1024)

typedef struct {
    F *f;
    FCGX_Request *request;
    ucontext_t ucontext;
    list_head wait_i_list;
} I;

#define INIT_I_LIST_HEAD(i) do{  \
    INIT_LIST_HEAD(&(i)->wait_i_list);\
}while(0);

void release_instance_space(I *i);

void make_request(F *f, FCGX_Request *req);


#endif //TPFAAS_INSTANCE_H
