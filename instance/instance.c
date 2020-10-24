//
// Created by kingdo on 2020/10/23.
//

#include <function/function.h>
#include <stdio.h>
#include <tool/queue.h>
#include "instance.h"

bool init_instance(I *i, F *f) {
    i->f = f;
    INIT_I_LIST_HEAD(i)
    return true;
}

bool malloc_instance_stack_when_create(I *i, void *arg, size_t arg_size) {
    MALLOC_DEFAULT_STACK(i->stack.stack_top, i->stack.stack_space);
    if (i->stack.stack_space == NULL) {
        printf("malloc stack space fault\n");
        return false;
    }
    COPY_STACK_DATA(i->stack.stack_top, arg, arg_size)
    i->stack.stack_size = DEFAULT_STACK_SIZE;
    return true;
}

void release_err_instance(I *i) {
    if (i->stack.stack_space != NULL)
        free(i->stack.stack_space);
    if (i != NULL)
        free(i);
}

I *create_instance(F *f, void *agr, size_t arg_size) {
    I *i = malloc(sizeof(I));
    if (i == NULL) {
        printf("malloc T space fault\n");
        return NULL;
    }
    if (init_instance(i, f) && malloc_instance_stack_when_create(i, agr, arg_size)) {
        return i;
    }
    release_err_instance(i);
    return NULL;
}

void add_2_F(I *i) {
    F_queue_list *fQueueList;
    //TODO 应该最好是倒查，让非wait的在前面，下面的操作要加锁了

    list_for_each_entry(fQueueList, &i->f->F_queue_list_head, F_queue_list_list){

    }
}

void receive_request(F *f, void *agr, size_t arg_size) {
    I *i = create_instance(f, agr, arg_size);
    add_2_F(i);
}