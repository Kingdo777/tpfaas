//
// Created by kingdo on 2020/10/23.
//
#include <function/function.h>
#include <stdio.h>
#include <pthread.h>
#include <task/task.h>
#include <sync/sync.h>
#include <malloc.h>
#include <tool/print.h>
#include "instance.h"


bool init_instance(I *i, F *f, int arg) {
    i->f = f;
    i->arg = arg;
    getcontext(&i->ucontext);
    i->ucontext.uc_stack.ss_sp = malloc(STACK_DEFAULT_SIZE);
    i->ucontext.uc_stack.ss_size = STACK_DEFAULT_SIZE;
    INIT_I_LIST_HEAD(i)
    return true;
}

I *create_instance(F *f, int arg) {
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

bool add_2_F_safe(I *i) {
    bool need_wake_T = false;
    if (i->f->concurrent_enable) {
        pthread_mutex_lock(&i->f->F_global_wait_i_lock);
        if (i->f->F_global_wait_i_list_rest_cap < 1) {
            need_wake_T = true;
            i->f->F_global_wait_i_list_rest_cap += i->f->concurrent_count;
            i->f->work_T_count++;
        }
        list_add(&i->wait_i_list, &i->f->F_global_wait_i_head);
        i->f->F_global_wait_i_list_rest_cap--;
        i->f->wait_I_count++;
        pthread_mutex_unlock(&i->f->F_global_wait_i_lock);
    } else {
        //直接唤醒一个T
        need_wake_T = true;
    }
    return need_wake_T;
}

void wake_T_for_I(I *i, F *f) {
    T *t;
    t = get_T_for_I(i, f);
    if (t != NULL) {
        //the T isn't new create,need week up it
        if (!f->concurrent_enable) {
            t->deal_with = i;
            t->direct_run = true;
        }
        t->work_for = f;
        put_T_into_R_busy_task_list_safe(t, f->r);
        thread_wake_up_one(t);
    }
}

//这里仅仅释放了I的空间,不包括I的栈空间
void release_instance_space(I *i) {
    if (i->ucontext.uc_stack.ss_sp != NULL) {
        free(i->ucontext.uc_stack.ss_sp);
    }
    if (i != NULL) {
        free(i);
    }
}

void make_request(F *f, int arg) {
    bool need_wake_T;
    I *i = create_instance(f, arg);
    if (NULL != i) {
        need_wake_T = add_2_F_safe(i);
        //此时,如果F是可并发的,也就意味着,i已经被放入了全局F队列中,也就说,I随时可能被拿去执行,然后被释放,因此v此时的I可能是free的
        //当然,如果F是不可并发的,那么没有就没有这个问题,因此当F可并发,i的信息就是危险的,无法使用
        if (need_wake_T) {
            wake_T_for_I(i, f);
        }
    }
}