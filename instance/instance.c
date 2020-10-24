//
// Created by kingdo on 2020/10/23.
//

#include <function/function.h>
#include <stdio.h>
#include <tool/queue.h>
#include <pthread.h>
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
    if (i->f->concurrent_enable) {
        F_global_I_queue *fQueueList;
        pthread_mutex_lock(&i->f->F_global_I_queue_lock);
        if (i->f->all_global_I_queue_rest_cap < 1) {
            //TODO 当前已经没有可用的位置了,说明已经到达了T的处理极限
        }
        list_for_each_entry(fQueueList, &i->f->F_global_I_queue_head, F_global_I_queue_list) {
            if (fQueueList->is_wait_queue &&
                fQueueList->i_queue->queue_list_size < fQueueList->i_queue->queue_list_max_cap) {

                goto unlock;
            }
        }
        //到这里说明wait_queue也满了,需要新建更多的wait_queue
        create_global_I_wait_queue_and_add_I_to_it(i->f, i);
        //TODO 唤醒操作!!!
        unlock:
        pthread_mutex_unlock(&i->f->F_global_I_queue_lock);
    } else {
        //TODO 否则应该直接找一个空闲的T,将I直接追加到T的deal_with上,而不需要如此繁琐的过程,这里是参考了open_whisk
    }

}

void receive_request(F *f, void *agr, size_t arg_size) {
    I *i = create_instance(f, agr, arg_size);
    add_2_F(i);
}