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
#include "tool/print.h"
#include "tool/stack.h"
#include "tool/list_head.h"
#include "tool/queue.h"
#include "task.h"

LIST_HEAD(task_list_head);

__pid_t task_pid_list__[TASK_MAX_COUNT];
int task_num__ = 0;

/**
 * get_instance 流程：
 *  若concurrent_enable==true:
 *    1. (1) 尝试从T的本地任务队列获取一个I-local（任务获取前，本地任务队列上锁<1>）
 *       (2) 判断T的本地任务队列的数目是否小于容量的一半？
 *           是：                                         （判断后，本地任务队列解锁<1> 全局任务队列上锁<1>）
 *              1）从F全局的I队列中尝试拉取(cap/2)数目的任务放到自己的本地队列中  （拉取之后，全局任务队列解锁<1>本地任务队列上锁<2>; 放入本地后，本地任务队列解锁<2>）
 *              2）判断I-local是否为NULL？
 *                 是：跳转到（3）
 *                 否：直接返回I-local，函数结束
 *           否：情况能保证I-local一定不为空，直接返回I-local，函数结束
 *       (3) 再次尝试从T的本地任务队列获取一个I-local（任务获取前，本地任务队列上锁<3>）
 *      （4） 无论I-local，是否为NULL，都直接返回    (任务获取后，本地任务队列解锁<3>）
 *  若concurrent_enable==false || concurrent_enable==true:(下面的操作则与之前绑定的F是否为并发F无关没有关系)
 *   2. (到达2,说明1的返回结果为NULL，即对应F的全局队列已经没有任何任务可以处理)
 *      (1) 遍历当前R的所有可并发F，判断其全局任务队列是否为空 （判断之前，该全局任务队列上锁<2>）
 *          若空：寻找下一个 （该全局任务队列解锁<2>）
 *          若不空：
 *              1) 修改自己的work_for
 *              2）从该F全局的I队列中尝试拉取(cap/2)数目的任务放到自己的本地队列中 （拉取之后，该全局任务队列解锁<2>本地任务队列上锁<4>; 放入本地后，本地任务队列解锁<4>）
 *              3) 从T的本地任务队列获取一个I-local（任务获取前，本地任务队列上锁<5> 任务获取后，本地任务队列解锁<5>）
 *              4）I-local一定不为NULL，返回，函数结束
 *      (2) 遍历结束，函数未返回，说明当前R的所有F都不存在任务可用
 *   3. 任务偷取
 *      (1) 遍历当前R的所有为可并发F服务的busy_T_list，判断其本地队列的大小 (判断之前，该本地任务队列上锁<1>）
 *          若空：寻找下一个 （该本地任务队列解锁<1>）
 *          若不空：
 *              1) 修改自己的work_for
 *              2）从该本地I队列中尝试拉取(list_current_count/2)数目的任务放到自己的本地队列中 (拉取之后，该本地任务队列解锁<1>本地任务队列上锁<6>; 放入本地后，本地任务队列解锁<6>）
 *              3) 从T的本地任务队列获取一个I-local（任务获取前，本地任务队列上锁<7> 任务获取后，本地任务队列解锁<7>）
 *              4）I-local一定不为NULL，返回，函数结束
 *      (2) 遍历结束，函数未返回，说明当前R的所有F都不存在任务可用
 *   4. 尽力了，当前R已经实在无任务可以执行，放入idle_task_list
 *
 *   note:
 *      we don't get I from other R'_F'_global_I_queue or  R'_busy_task_list,
 *      because when a busy_R need an idle_T, it will steal one from other R.
 *
 *
 *
 * */

I *take_I_from_I_local_task_queue_unsafe(T *t) {
    I *i;
    if (t->i_queue->i_queue->queue_list_size > 0) {
        take_an_entry_from_tail(i, &t->i_queue->i_queue->instance_list_head, I_local_wait_queue_list)
        list_del(&i->I_local_wait_queue_list);
        t->i_queue->i_queue->queue_list_size--;
    }
    return i;
}

I *take_I_from_I_local_task_queue_safe(T *t) {
    I *i;
    pthread_mutex_lock(&t->T_local_I_queue_lock);
    i = take_I_from_I_local_task_queue_unsafe(t);
    pthread_mutex_unlock(&t->T_local_I_queue_lock);
    return i;
}

int cut_n_T_from_F_global_task_list_unsafe(T *t, list_head_chain *l_chain, int count) {
    int real_count = 0, tmp_count = 0;
    F_global_I_queue *fGlobalIQueue = NULL;
    F *f = t->work_for;
    list_for_each_entry(fGlobalIQueue, &f->F_global_I_queue_head, F_global_I_queue_list) {
        if (fGlobalIQueue->i_queue->queue_list_size > 0 &&
            !list_empty(&fGlobalIQueue->i_queue->instance_list_head)) {
            tmp_count = try_take_a_N_size_chain_from_list(l_chain, &fGlobalIQueue->i_queue->instance_list_head, count);
            real_count += tmp_count;
            count -= real_count;
            fGlobalIQueue->i_queue->queue_list_size -= tmp_count;
            f->wait_I_count -= tmp_count;
            tmp_count = 0;
            if (count == 0) {
                break;
            }
        }
    }
    return real_count;
}

int cut_n_T_from_T_local_task_list_unsafe(T *t, list_head_chain *l_chain, int count) {
    int real_count = 0;
    real_count = try_take_a_N_size_chain_from_list(l_chain, &t->i_queue->i_queue->instance_list_head, count);
    t->i_queue->i_queue->queue_list_size -= count;
    return real_count;
}

void append_n_T_to_T_local_task_list_safe(T *t, list_head_chain *l_chain, int count) {
    F *f = t->work_for;
    pthread_mutex_lock(&t->T_local_I_queue_lock);
    t->i_queue->i_queue->queue_list_max_cap = f->concurrent_count;
    append_a_list_chain_2_list_head(l_chain, &t->i_queue->i_queue->instance_list_head);
    t->i_queue->i_queue->queue_list_size += count;
    pthread_mutex_unlock(&t->T_local_I_queue_lock);
}

int try_get_n_I_from_F_global_task_list_2_T_local_task_list_safe(T *t, int count) {
    F *f = t->work_for;
    pthread_mutex_lock(&f->F_global_I_queue_lock);
    list_head_chain l_chain;
    int pull_count = count < f->wait_I_count ? count : f->wait_I_count;
    if (pull_count > 0) {
        pull_count = cut_n_T_from_F_global_task_list_unsafe(t, &l_chain, pull_count);
        pthread_mutex_unlock(&f->F_global_I_queue_lock);
    } else {
        pthread_mutex_unlock(&f->F_global_I_queue_lock);
        return 0;
    }
    append_n_T_to_T_local_task_list_safe(t, &l_chain, pull_count);
    return pull_count;
}

int try_get_n_I_from_other_T_local_task_list_2_T_local_task_list_safe(T *t, T *other_t) {
    pthread_mutex_lock(&other_t->T_local_I_queue_lock);
    int pull_count = other_t->i_queue->i_queue->queue_list_size / 2;
    list_head_chain l_chain;
    if (pull_count > 0) {
        pull_count = cut_n_T_from_T_local_task_list_unsafe(t, &l_chain, pull_count);
        pthread_mutex_unlock(&other_t->T_local_I_queue_lock);
    } else {
        pthread_mutex_unlock(&other_t->T_local_I_queue_lock);
        return 0;
    }
    append_n_T_to_T_local_task_list_safe(t, &l_chain, pull_count);
    return pull_count;
}

/**
 * 实现步骤1：
 * */
I *get_I_from_I_local_task_queue__and__supply_I_from_F_if_need(T *t) {
    if (!t->work_for->concurrent_enable) {
        return NULL;
    }
    I *i = NULL;
    pthread_mutex_lock(&t->T_local_I_queue_lock);
    //// 1. (1)
    i = take_I_from_I_local_task_queue_unsafe(t);
    //// 1. (2)
    bool need_supply_I_from_F = (t->i_queue->i_queue->queue_list_size <= t->i_queue->i_queue->queue_list_max_cap);
    pthread_mutex_unlock(&t->T_local_I_queue_lock);
    //// 1. (2) 是：
    if (need_supply_I_from_F) {
        //// 1. (2) 是  1）
        try_get_n_I_from_F_global_task_list_2_T_local_task_list_safe(t, t->work_for->concurrent_count / 2);
        //// 1. (2) 是  2）
        if (NULL != i) {
            //// 1. (2) 是  2）否
            return i;
        }
    } else {
        //// 1. (2) 否：
        if (i == NULL) {
            never_reach("can't get instance - q1\n");
        }
        return i;
    }
    //// 1. (2) 是  2）是
    //// 1. (3)
    i = take_I_from_I_local_task_queue_safe(t);
    //// 1. (4)
    return i;
}

/**
 * 实现步骤2 (具体实现上处于代码优化的考虑和算法并不完全一致)：
 * */
I *get_instance_form_R_all_F_global_queue(T *t) {
    I *i = NULL;
    F *f;
    R *r = t->work_for->r;
    list_for_each_entry(f, &r->function_head, func_list) {
        if (f->concurrent_enable) {
            t->work_for = f;
            if (try_get_n_I_from_F_global_task_list_2_T_local_task_list_safe(t, t->work_for->concurrent_count / 2) >
                0) {
                i = take_I_from_I_local_task_queue_safe(t);
                if (i == NULL) {
                    never_reach("can't get instance - q2\n");
                }
                return i;
            }
        }
    }
    return i;
}

/**
 * 实现步骤3：
 * */
I *steal_instance_form_R_all_busy_T_local_queue(T *t) {
    I *i = NULL;
    F *f;
    R *r = t->work_for->r;
    T *other_t;
    list_for_each_entry(other_t, &r->task_busy_head, task_busy_list) {
        f = other_t->work_for;
        if (f->concurrent_enable) {
            t->work_for = f;
            if (try_get_n_I_from_other_T_local_task_list_2_T_local_task_list_safe(t, other_t) >
                0) {
                i = take_I_from_I_local_task_queue_safe(t);
                if (i == NULL) {
                    never_reach("can't get instance - q3\n");
                }
                return i;
            }
        }
    }
    return i;
}

void get_instance(T *t) {
    I *i;
    find:
    if (t->deal_with != NULL && t->work_for != NULL && !t->work_for->concurrent_enable && t->direct_run) {
        t->direct_run = false;
        return;
    }
    i = get_I_from_I_local_task_queue__and__supply_I_from_F_if_need(t);
    if (NULL == i) {
        i = get_instance_form_R_all_F_global_queue(t);
    }
    if (NULL == i) {
        i = steal_instance_form_R_all_busy_T_local_queue(t);
    }
    if (NULL != i) {
        t->work_for = i->f;
        t->deal_with = i;
        return;
    }
    remove_T_from_R_busy_task_list_safe(t, t->work_for->r);
    put_T_into_R_idle_task_list_safe(t, t->work_for->r);
    thread_sleep(&t->futex_word);
    goto find;
}

void task_done() {
    TLS(t);
    if (t->deal_with != NULL && !t->direct_run) {
        //清空执行Instance所保留的栈数据
        release_task_stack_when_sleep(t);
        //删除instance
        release_instance_space(t->deal_with);
    }
    //get_function如果找不到function，那么将一直睡眠，一旦返回说明已经找到
    get_instance(t);
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
            put_T_into_R_busy_task_list_safe(t, i->f->r);
        }
        return t;
    }
    //错误处理：从busy队列中移除，并释放栈空间
    if (i != NULL) {
        remove_T_from_R_busy_task_list_safe(t, i->f->r);
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
    t->direct_run = (((i == NULL || i->f->concurrent_enable) ? false : true));
    if (!malloc_task_stack_when_create(t))
        return false;
    INIT_T_LIST_HEAD(t)

    t->i_queue = malloc(sizeof(T_local_I_list));
    t->i_queue->i_queue = malloc(sizeof(instance_queue));
    t->i_queue->i_queue->queue_list_size = 0;
    t->i_queue->i_queue->queue_list_max_cap = (((i == NULL || !i->f->concurrent_enable) ? 0 : i->f->concurrent_count));
    INIT_LIST_HEAD(&t->i_queue->i_queue->instance_list_head);

    pthread_mutex_init(&t->T_local_I_queue_lock, NULL);
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
    gogo_switch_new_free_old(t->stack.stack_top, old_stack_space);
}

bool malloc_task_stack_when_create(T *t) {
//    MALLOC_DEFAULT_STACK(t->stack.stack_top, t->stack.stack_space)
    do {
        t->stack.stack_space = malloc(8 * 1024);
        t->stack.stack_top = t->stack.stack_space + 8 * 1024;
    } while (0);
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
//    malloc_instance_stack_when_create(t->deal_with, "Hello clone\n", sizeof("Hello clone\n"));
    t->tgid = clone((void *) task_done, t->stack.stack_top,
                    CLONE_VM |
                    CLONE_SETTLS |
                    CLONE_CHILD_SETTID |
                    CLONE_PARENT_SETTID,
                    NULL, &task_pid_list__[task_num__++], t, &t->tgid);

    return t->tgid == -1;
}

void put_T_into_R_idle_task_list_safe(T *t, R *r) {
    pthread_mutex_lock(&r->task_idle_lock);
    t->work_for = NULL;
    t->deal_with = NULL;
    list_add(&t->task_idle_list, &r->task_idle_head);
    r->task_idle_count++;
    pthread_mutex_unlock(&r->task_idle_lock);
}

void remove_T_from_R_idle_task_list_safe(T *t, R *r) {
    pthread_mutex_lock(&r->task_idle_lock);
    if (r->task_idle_count > 0) {
        list_del(&t->task_idle_list);
        r->task_idle_count--;
    }
    pthread_mutex_unlock(&r->task_idle_lock);
}

void put_T_into_R_busy_task_list_safe(T *t, R *r) {
    pthread_mutex_lock(&r->task_busy_lock);
    list_add(&t->task_busy_list, &r->task_busy_head);
    r->task_busy_count++;
    pthread_mutex_unlock(&r->task_busy_lock);
}

void remove_T_from_R_busy_task_list_safe(T *t, R *r) {
    pthread_mutex_lock(&r->task_busy_lock);
    if (r->task_busy_count > 0) {
        list_del(&t->task_busy_list);
        r->task_busy_count--;
    }
    pthread_mutex_unlock(&r->task_busy_lock);
    //TODO 需要修改T的资源配置
}

T *get_T_from_R_idle_task_list_safe(R *r) {
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
            t = get_T_from_R_idle_task_list_safe(r);
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
    t = get_T_from_R_idle_task_list_safe(r);
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