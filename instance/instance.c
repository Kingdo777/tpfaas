//
// Created by kingdo on 2020/10/23.
//
#include <function/function.h>
#include <stdio.h>
#include <tool/queue.h>
#include <pthread.h>
#include <task/task.h>
#include <sync/sync.h>
#include <malloc.h>
#include <tool/print.h>
#include "instance.h"


bool init_instance(I *i, F *f, void *arg) {
    i->f = f;
    i->arg = arg;
    INIT_I_LIST_HEAD(i)
    return true;
}

I *create_instance(F *f, void *arg) {
    I *i = malloc(sizeof(I));
    if (i == NULL) {
        printf("malloc T space fault\n");
        return NULL;
    }
    if (init_instance(i, f, arg)) {
        return i;
    }
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
        //the T isn't new create,need week up it
        if (!i->f->concurrent_enable) {
            t->deal_with = i;
            t->direct_run = true;
        }
        t->work_for = i->f;
        put_T_into_R_busy_task_list_safe(t, i->f->r);
        thread_wake_up_one(t);
    }
}

//这里仅仅释放了I的空间,不包括I的栈空间
void release_instance_space(I *i) {
    if (i != NULL) {
        free(i);
    }
}

void make_request(F *f, void *agr) {
    bool need_wake_T;
    I *i = create_instance(f, agr);
    if (NULL != i) {
        need_wake_T = add_2_F(i);
        if (need_wake_T) {
            wake_T_for_I(i);
        }
    }
}