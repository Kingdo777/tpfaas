//
// Created by kingdo on 10/20/20.
//
#define _GNU_SOURCE

#include <sched.h>
#include "tool/tls.h"
#include "sync/sync.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <tool/print.h>
#include "tool/stack.h"
#include "tool/queue.h"
#include "task.h"

LIST_HEAD(task_list_head);

__pid_t task_pid_list__[TASK_MAX_COUNT];
int task_num__ = 0;

I *get_instance_form_R(R *r) {
    I *i = NULL;
    F *f;
    F_global_I_queue *fGlobalIQueue = NULL;
    list_for_each_entry(f, &r->function_head, func_list) {
        if (f->concurrent_enable) {
            pthread_mutex_lock(&f->F_global_I_queue_lock);
            if (f->wait_I_count > 0) {
                list_for_each_entry(fGlobalIQueue, &f->F_global_I_queue_head, F_global_I_queue_list) {
                    if (fGlobalIQueue->i_queue->queue_list_size > 0 &&
                        !list_empty(&fGlobalIQueue->i_queue->instance_list_head)) {
                        take_an_entry_from_head(i, &fGlobalIQueue->i_queue->instance_list_head,
                                                F_global_wait_queue_list)
                        list_del(&i->F_global_wait_queue_list);
                        fGlobalIQueue->i_queue->queue_list_size--;
                        f->wait_I_count--;
                        break;
                    }
                }
            }
            pthread_mutex_unlock(&f->F_global_I_queue_lock);
        }
        if (NULL != i) {
            break;
        }
    }
    return i;
}

I *get_instance_form_all_R_except_r1(R *r1) {
    R *r;
    I *i = NULL;
    list_for_each_entry(r, &res_list_head, res_list) {
        if (r != r1) {
            i = get_instance_form_R(r);
            if (NULL != i)
                break;
        }
    }
    return i;
}

void get_instance() {
    TLS(t);
    I *i;
    find:
    if (t->deal_with != NULL && t->work_for != NULL)
        return;
    if (t->work_for->concurrent_enable) {

        return;
    }
    i = get_instance_form_R(t->work_for->r);
    if (NULL == i)
        i = get_instance_form_all_R_except_r1(t->work_for->r);
    if (NULL != i) {
        t->work_for = i->f;
        t->deal_with = i;
        return;
    }
    remove_T_from_R_busy_task_list(t, t->work_for->r);
    put_T_into_R_idle_task_list(t, t->work_for->r);
    thread_sleep(&t->futex_word);
    goto find;
}

void task_done() {
    TLS(t);
    //清空执行Instance所保留的栈数据
    release_task_stack_when_sleep(t);
    //get_function如果找不到function，那么将一直睡眠，一旦返回说明已经找到
    get_instance();
    //找到的instance被写到了task的deal_with中
    gogo(t);
}

//i可以为NULL
T *creat_T(I *i) {
    T *t = malloc(sizeof(T));
    if (t == NULL) {
        printf("malloc T space fault\n");
        return NULL;
    }
    if (init_task(t, i) && bind_os_thread(t)) {
        //只要不为空就放到busy队列
        if (i != NULL) {
            put_T_into_R_busy_task_list(t, i->f->r);
        }
        return t;
    }
    //错误处理：从busy队列中移除，并释放栈空间
    if (i != NULL) {
        remove_T_from_R_busy_task_list(t, i->f->r);
    }
    release_err_task(t);
    return NULL;
}


bool init_task(T *t, I *i) {
    t->futex_word = DEFAULT_FUTEX_WORD;
    //如果开启了并发，那么将通过get函数寻找I，并拉去一定数目的I
    //如果未开启，那么直接制定deal_with，可以立马执行
    t->work_for = (i == NULL ? NULL : i->f);
    t->deal_with = (((i == NULL || i->f->concurrent_enable) ? NULL : i));
    if (!malloc_task_stack_when_create(t))
        return false;
    INIT_T_LIST_HEAD(t)
    t->F_local_wait_queue_size = 0;
    pthread_mutex_init(&t->F_local_wait_queue_lock, NULL);
    list_add(&t->task_list, &task_list_head);
    return true;
}

void release_err_task(T *t) {
    if (t->stack.stack_space != NULL)
        free(t->stack.stack_space);
    if (t != NULL)
        free(t);
}

void release_task_stack_when_sleep(T *t) {
    void *old_stack_space = t->stack.stack_space;
    malloc_task_stack_when_create(t);
    gogo_switch(t->stack.stack_top);
    free(old_stack_space);
}

bool malloc_task_stack_when_create(T *t) {
    MALLOC_DEFAULT_STACK(t->stack.stack_top, t->stack.stack_space);
    if (t->stack.stack_space == NULL) {
        printf("malloc stack space fault\n");
        return false;
    }
    t->stack.stack_size = DEFAULT_STACK_SIZE;
    return true;
}

bool bind_os_thread(T *t) {
    int try_c = TRY_COUNT;
    try:
    if (bind_os_thread_(t)) {
        printf("clone wrong,try %d\n", TRY_COUNT - try_c + 1);
        if (try_c-- > 0)
            goto try;
        printf("clone fault,errno:%d：\n", errno);
        return false;
    }
    return true;
}

bool bind_os_thread_(T *t) {
    /**
         * 这里仅仅设置了CLONE_VM参数而没有设置CLONE_THREAD flag是一个重大的决定，这意味着，我们clone的线程和posix定义的线程不同
         * 可以认为我们定义的是一个共享内存地址空间的进程，这样clone的每个进程依然是一个线程组，但是exec、exit仍然还是一个危险的操作
         *
         * CLONE_SETTLS 使用的也是arch_prctl系统调用，利用FS实现线程存储
         *
         * SIGCHLD，子进程退出时，向父进程发送的signal，如果不发送这个信号，那么是没办法被wait()获取的，只能使用waitpid + __WALL/__WCLONE等参数
         *
        */
    t->tgid = clone((void *) task_done, t->stack.stack_top,
                    CLONE_VM |
                    CLONE_SETTLS |
                    CLONE_CHILD_SETTID |
                    CLONE_PARENT_SETTID,
                    NULL, &task_pid_list__[task_num__++], t, &t->tgid);
    return t->tgid != -1;
}

void put_T_into_R_idle_task_list(T *t, R *r) {
    pthread_mutex_lock(&r->task_idle_lock);
    t->work_for = NULL;
    t->deal_with = NULL;
    list_add(&t->task_idle_list, &r->task_idle_head);
    r->task_idle_count++;
    pthread_mutex_unlock(&r->task_idle_lock);
}

void remove_T_from_R_idle_task_list(T *t, R *r) {
    pthread_mutex_lock(&r->task_idle_lock);
    if (r->task_idle_count > 0) {
        list_del(&t->task_idle_list);
        r->task_idle_count--;
    }
    pthread_mutex_unlock(&r->task_idle_lock);
}

void put_T_into_R_busy_task_list(T *t, R *r) {
    pthread_mutex_lock(&r->task_busy_lock);
    list_add(&t->task_busy_list, &r->task_busy_head);
    r->task_busy_count++;
    pthread_mutex_unlock(&r->task_busy_lock);
}

void remove_T_from_R_busy_task_list(T *t, R *r) {
    pthread_mutex_lock(&r->task_busy_lock);
    if (r->task_busy_count > 0) {
        list_del(&t->task_busy_list);
        r->task_busy_count--;
    }
    pthread_mutex_unlock(&r->task_busy_lock);
}

T *get_T_from_R_idle_task_list(R *r) {
    pthread_mutex_lock(&r->task_idle_lock);
    if (r->task_idle_count > 0) {
        T *t;
        list_for_each_entry(t, &r->task_idle_head, task_idle_list) {
            list_del(&t->task_idle_list);
            r->task_idle_count--;
            return t;
        }
        never_reach("cant get idle task\n");
    } else {
        return NULL;
    }
    pthread_mutex_unlock(&r->task_idle_lock);
}

T *get_T_from_all_R_idle_task_list_except_R1(R *r1) {
    R *r;
    T *t = NULL;
    list_for_each_entry(r, &res_list_head, res_list) {
        if (r != r1) {
            t = get_T_from_R_idle_task_list(r);
            if (NULL != t)
                return t;
        }
    }
    return t;
}

//这里返回的T一定是纯净的，不在任何链表中的(idle\busy\work_for_F)
//如果返回的是NULL说明是新创建的。直接为I服务不需要唤醒
T *get_T_for_I(I *i) {
    T *t;
    R *r = i->f->r;
    t = get_T_from_R_idle_task_list(r);
    if (NULL == t) {
        t = get_T_from_all_R_idle_task_list_except_R1(r);
    }
    if (NULL == t) {
        creat_T(i);
        //返回NULL是因为T已经在很好的运行，不需要唤醒
        return NULL;
    }
    return t;
}