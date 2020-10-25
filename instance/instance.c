//
// Created by kingdo on 2020/10/23.
//

#include <function/function.h>
#include <stdio.h>
#include <tool/queue.h>
#include <pthread.h>
#include <tool/print.h>
#include <task/task.h>
#include <sync/sync.h>
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

bool add_2_F(I *i) {
    bool need_wake_T = false;
    if (i->f->concurrent_enable) {
        F_global_I_queue *fQueueList;
        pthread_mutex_lock(&i->f->F_global_I_queue_lock);
        if (i->f->all_global_I_queue_rest_cap > 1) {
            list_for_each_entry_prev(fQueueList, &i->f->F_global_I_queue_head, F_global_I_queue_list) {
                //这里要倒序是因为run_queue一般是放在末尾的
                if (!fQueueList->is_wait_queue) {
                    fQueueList->i_queue->queue_list_size++;
                    goto unlock;
                }
            }
            //never reach
            never_reach("can't find available run_queue\n");
        }
        bool if_create_queue = false;
        put_I_into_wait_queue:
        list_for_each_entry(fQueueList, &i->f->F_global_I_queue_head, F_global_I_queue_list) {
            if (fQueueList->is_wait_queue &&
                fQueueList->i_queue->queue_list_size < fQueueList->i_queue->queue_list_max_cap) {
                list_add(&i->F_global_wait_queue_list, &i->f->F_global_I_queue_head);
                fQueueList->i_queue->queue_list_size++;
                if (if_create_queue) {
                    goto wake_T;
                }
                goto unlock;
            }
        }
        create_wait_queue:
        //到这里说明wait_queue也满了,需要新建更多的wait_queue
        create_global_I_wait_queue(i->f);
        if_create_queue = true;
        goto put_I_into_wait_queue;
        wake_T:
        //之所以不在这里进行唤醒，是避免锁的嵌套获取
        need_wake_T = true;
        unlock:
        i->f->wait_I_count++;
        pthread_mutex_unlock(&i->f->F_global_I_queue_lock);
    } else {
        //直接唤醒一个T
        need_wake_T = true;
    }
    return need_wake_T;
}

void wake_T_for_I(I *i) {
    T *t;
    t = get_T_for_I(i);
    if (t != NULL) {
        //the T isn't new create,need sleep it
        if (!i->f->concurrent_enable) {
            t->deal_with = i;
            t->direct_run = true;
        }
        t->work_for = i->f;
        put_T_into_R_busy_task_list(t, i->f->r);
        thread_wake_up_one(t);
    }
}

void receive_request(F *f, void *agr, size_t arg_size) {
    bool need_wake_T;
    I *i = create_instance(f, agr, arg_size);
    if (NULL != i) {
        need_wake_T = add_2_F(i);
        if (need_wake_T) {
            wake_T_for_I(i);
        }
    }
}