//
// Created by kingdo on 10/20/20.
//
#define _GNU_SOURCE

#include <sched.h>
#include "sync/sync.h"
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <tool/print.h>
#include "tool/list_head.h"
#include "task.h"

LIST_HEAD(task_list_head)

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
    I *i = NULL;
    if (t->T_local_wait_i_size > 0) {
        take_an_entry_from_tail(i, &t->T_local_wait_i_head, wait_i_list)
        list_del(&i->wait_i_list);
        t->T_local_wait_i_size--;
    }
    return i;
}

I *take_I_from_I_local_task_queue_safe(T *t) {
    I *i = NULL;
    pthread_mutex_lock(&t->T_local_wait_i_lock);
    i = take_I_from_I_local_task_queue_unsafe(t);
    pthread_mutex_unlock(&t->T_local_wait_i_lock);
    return i;
}

int cut_n_T_from_F_global_task_list_unsafe(T *t, list_head_chain *l_chain, int count) {
    int real_count;
    F *f = t->work_for;
    real_count = try_take_a_N_size_chain_from_list(l_chain, &f->F_global_wait_i_head, count);
    f->wait_I_count -= real_count;
    return real_count;
}

int cut_n_T_from_T_local_task_list_unsafe(T *t, list_head_chain *l_chain, int count) {
    int real_count;
    real_count = try_take_a_N_size_chain_from_list(l_chain, &t->T_local_wait_i_head, count);
    t->T_local_wait_i_size -= count;
    return real_count;
}

void append_n_T_to_T_local_task_list_safe(T *t, list_head_chain *l_chain, int count) {
    if (count > 0) {
        pthread_mutex_lock(&t->T_local_wait_i_lock);
        append_a_list_chain_2_list_head(l_chain, &t->T_local_wait_i_head);
        t->T_local_wait_i_size += count;
        pthread_mutex_unlock(&t->T_local_wait_i_lock);
    }
}

int try_get_n_I_from_F_global_task_list_2_T_local_task_list_safe(T *t, int count) {
    F *f = t->work_for;
    pthread_mutex_lock(&f->F_global_wait_i_lock);
    list_head_chain l_chain;
    int pull_count = count < f->wait_I_count ? count : f->wait_I_count;
    if (pull_count > 0) {
        pull_count = cut_n_T_from_F_global_task_list_unsafe(t, &l_chain, pull_count);
        pthread_mutex_unlock(&f->F_global_wait_i_lock);
    } else {
        pthread_mutex_unlock(&f->F_global_wait_i_lock);
        return 0;
    }
    append_n_T_to_T_local_task_list_safe(t, &l_chain, pull_count);
    return pull_count;
}

int try_get_n_I_from_other_T_local_task_list_2_T_local_task_list_safe(T *t, T *other_t) {
    pthread_mutex_lock(&other_t->T_local_wait_i_lock);
    int pull_count = other_t->T_local_wait_i_size / 2;
    list_head_chain l_chain;
    if (pull_count > 0) {
        pull_count = cut_n_T_from_T_local_task_list_unsafe(other_t, &l_chain, pull_count);
        pthread_mutex_unlock(&other_t->T_local_wait_i_lock);
    } else {
        pthread_mutex_unlock(&other_t->T_local_wait_i_lock);
        return 0;
    }
    append_n_T_to_T_local_task_list_safe(t, &l_chain, pull_count);
    return pull_count;
}

void T_attach_to_F_safe(F *f) {
    if (f->concurrent_enable) {
        pthread_mutex_lock(&f->F_global_wait_i_lock);
        f->work_T_count++;
        f->F_global_wait_i_list_rest_cap += f->concurrent_count;
        pthread_mutex_unlock(&f->F_global_wait_i_lock);
    }
}

void T_leave_from_F_safe(F *f) {
    if (f->concurrent_enable) {
        pthread_mutex_lock(&f->F_global_wait_i_lock);
        f->work_T_count--;
        f->F_global_wait_i_list_rest_cap -= f->concurrent_count;
        pthread_mutex_unlock(&f->F_global_wait_i_lock);
    }
}

/**
 * 实现步骤1：
 * */
I *get_I_from_I_local_task_queue__and__supply_I_from_F_if_need(T *t) {
    if (!t->work_for->concurrent_enable) {
        return NULL;
    }
    I *i = NULL;
    pthread_mutex_lock(&t->T_local_wait_i_lock);
    //// 1. (1)
    i = take_I_from_I_local_task_queue_unsafe(t);
    //// 1. (2)
    bool need_supply_I_from_F = (t->T_local_wait_i_size <= t->work_for->concurrent_count / 2);
    pthread_mutex_unlock(&t->T_local_wait_i_lock);
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
    if (NULL == i) {
        T_leave_from_F_safe(t->work_for);
    }
    return i;
}

/**
 * 实现步骤2 (具体实现上处于代码优化的考虑和算法并不完全一致)：
 * */
I *get_instance_form_R_all_F_global_queue(T *t) {
    I *i = NULL;
    F *f;
    R *r = t->work_for->r;
    list_for_each_entry(f, &r->resource_func_head, resource_func_list) {
        if (f->concurrent_enable) {
            t->work_for = f;
            if (try_get_n_I_from_F_global_task_list_2_T_local_task_list_safe(t, t->work_for->concurrent_count / 2) >
                0) {
                i = take_I_from_I_local_task_queue_safe(t);
                if (i != NULL) {
                    T_attach_to_F_safe(f);
                    return i;
                }
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
    F *f = NULL;
    R *r = t->work_for->r;
    T *other_t;
    ////这里进行了锁的嵌套,是比较耗时的操作
    pthread_mutex_lock(&r->task_busy_lock);
    if (r->task_busy_count > 0) {
        list_for_each_entry(other_t, &r->task_busy_head, task_busy_list) {
            f = other_t->work_for;
            if (f->concurrent_enable && other_t->T_local_wait_i_size > f->concurrent_count / 2) {
                t->work_for = f;
                if (try_get_n_I_from_other_T_local_task_list_2_T_local_task_list_safe(t, other_t) >
                    0) {
                    i = take_I_from_I_local_task_queue_safe(t);
                    if (i != NULL) {
                        T_attach_to_F_safe(f);
                        pthread_mutex_unlock(&r->task_busy_lock);
                        return i;
                    }
                }
            }
        }
    }
    pthread_mutex_unlock(&r->task_busy_lock);
    return i;
}

void get_instance(T *t) {
    I *i = NULL;
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
    //这个地方那个存在一个FUTEX的BUG,当把一个T放入idle之后,这个T马上就会被感知的,并被从idle中拿出,并尝试唤醒,由于此时还没有进行sleep,
    //这将导致,唤醒操作没有意义,且这个T在执行睡眠后将无法在被唤醒
    //这个Bug暂时无解,因为,要想保证上述情况只能加锁,但是睡眠之后无法解锁,因此锁机制没法用,目前只能使用信号量来解决
    put_T_into_R_idle_task_list_safe(t, t->work_for->r);
    thread_sleep(t);
    goto find;
}

_Noreturn void *task_done_(T *t) {
    handle_request:
    if (t->deal_with != NULL && !t->direct_run) {
        if (t->work_for->concurrent_enable) {
            pthread_mutex_lock(&t->work_for->F_global_wait_i_lock);
            t->work_for->F_global_wait_i_list_rest_cap++;
            pthread_mutex_unlock(&t->work_for->F_global_wait_i_lock);
        }
        //删除instance
        release_instance_space(t->deal_with);
    }
    //get_function如果找不到function，那么将一直睡眠，一旦返回说明已经找到
    get_instance(t);
    //找到的instance被写到了task的deal_with中
    I *i = t->deal_with;
    i->ucontext.uc_link = &t->task_context;
    makecontext(&i->ucontext, (void (*)(void)) i->f->entry_addr, 1, (int) i->arg);
    setcontext(&t->deal_with->ucontext);
    goto handle_request;
}

_Noreturn void *task_done(void *arg) {
    T *t = (T *) arg;
    getcontext(&t->task_context);
    task_done_(t);
}

//i可以为NULL
T *creat_T(I *i, F *f) {
    T *t = malloc(sizeof(T));
    if (t == NULL) {
        printf("malloc T space fault\n");
        return NULL;
    }
    if (init_task(t, i, f)) {
        put_T_into_R_busy_task_list_safe(t, f->r);
        //保证在执行子线程之前，数据都是正确写入的，因此在此函数后面父进程不应该再执行任何有意义的操作
        if (bind_os_thread(t)) {
            return t;
        }
    }
    //到这里说明存在创建的错误
    //错误处理：从busy队列中移除，并栈空间
    remove_T_from_R_busy_task_list_safe(t, f->r);
    free(t);
    return NULL;
}


bool init_task(T *t, I *i, F *f) {
    pthread_mutex_init(&t->t_mutex, NULL);
    pthread_cond_init(&t->t_cont, NULL);
    //如果开启了并发，那么将通过get函数寻找I，并拉去一定数目的I
    //如果未开启，那么直接制定deal_with，可以立马执行
    t->futex_word = DEFAULT_FUTEX_WORD;
    sem_init(&t->sem_word, 0, 0);
    t->status = None;
    t->work_for = f;
    t->deal_with = (((i == NULL || f->concurrent_enable) ? NULL : i));
    t->direct_run = (((i == NULL || f->concurrent_enable) ? false : true));
    INIT_T_LIST_HEAD(t)
    t->T_local_wait_i_size = 0;
    pthread_mutex_init(&t->T_local_wait_i_lock, NULL);
    return true;
}


bool bind_os_thread(T *t) {
    int try_c = TRY_COUNT;
    try:
    if (!bind_os_thread_(t)) {
        printf("clone wrong,try %d\n", TRY_COUNT - try_c + 1);
        if (try_c-- > 0)
            goto try;
        printf("clone fault,errno:%d：\n", errno);
        return false;
    }
    return true;
}

bool bind_os_thread_(T *t) {
    int ret;
    ret = pthread_create(&t->tgid, NULL, task_done, t);
    if (ret != 0) {
        switch (ret) {
            case EAGAIN:
                fprintf(stderr,
                        "pthread_create wrong:A system-imposed limit on the number of threads was encountered or "
                        "Insufficient resources to create another thread." "\n");
                break;
            case EINVAL:
                fprintf(stderr, "pthread_create wrong:Invalid settings in attr." "\n");
                break;
            case EPERM:
                fprintf(stderr,
                        "pthread_create wrong:No permission to set the scheduling policy and parameters specified in attr." "\n");
                break;
            default:
                fprintf(stderr, "pthread_create wrong:unknown wrong." "\n");
                break;
        }
    }
    return ret == 0;
}

void put_T_into_R_idle_task_list_safe(T *t, R *r) {
    if (NULL == t || None != t->status)
        never_reach("can't put_T_into_R_idle_task_list_safe right\n");
    pthread_mutex_lock(&r->task_idle_lock);
    t->work_for = NULL;
    t->deal_with = NULL;
    t->status = Idle;
    list_add(&t->task_idle_list, &r->task_idle_head);
    r->task_idle_count++;
    pthread_mutex_unlock(&r->task_idle_lock);
}

void remove_T_from_R_idle_task_list_safe(T *t, R *r) {
    if (NULL == t || Idle != t->status)
        never_reach("can't remove_T_from_R_idle_task_list_safe right\n");
    if (r->task_idle_count > 0) {
        pthread_mutex_lock(&r->task_idle_lock);
        if (r->task_idle_count > 0) {
            t->status = None;
            list_del(&t->task_idle_list);
            r->task_idle_count--;
        }
        pthread_mutex_unlock(&r->task_idle_lock);
    }
}

void put_T_into_R_busy_task_list_safe(T *t, R *r) {
    if (NULL == t || None != t->status)
        never_reach("can't put_T_into_R_busy_task_list_safe right\n");
    pthread_mutex_lock(&r->task_busy_lock);
    t->status = Busy;
    list_add(&t->task_busy_list, &r->task_busy_head);
    r->task_busy_count++;
    pthread_mutex_unlock(&r->task_busy_lock);
}

void remove_T_from_R_busy_task_list_safe(T *t, R *r) {
    if (NULL == t || Busy != t->status)
        never_reach("can't remove_T_from_R_busy_task_list_safe right\n");
    if (r->task_busy_count > 0) {
        pthread_mutex_lock(&r->task_busy_lock);
        if (r->task_busy_count > 0) {
            t->status = None;
            list_del(&t->task_busy_list);
            r->task_busy_count--;
        }
        pthread_mutex_unlock(&r->task_busy_lock);
    }
    //TODO 需要修改T的资源配置
}

T *get_T_from_R_idle_task_list_safe(R *r) {
    T *t = NULL;
    if (r->task_idle_count > 0) {
        pthread_mutex_lock(&r->task_idle_lock);
        if (r->task_idle_count > 0) {
            take_an_entry_from_head(t, &r->task_idle_head, task_idle_list)
            if (NULL == t || Idle != t->status)
                never_reach("can't get_T_from_R_idle_task_list_safe right\n");
            t->status = None;
            list_del(&t->task_idle_list);
            r->task_idle_count--;
        }
        pthread_mutex_unlock(&r->task_idle_lock);
    }
    return t;
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
T *get_T_for_I(I *i, F *f) {
    T *t = NULL;
    R *r = f->r;
    t = get_T_from_R_idle_task_list_safe(r);
    if (NULL == t) {
        t = get_T_from_all_R_idle_task_list_except_R1(r);
    }
    if (NULL == t) {
        if (f->concurrent_enable) {
            creat_T(NULL, f);
        } else {
            creat_T(i, f);
        }
        //返回NULL是因为T已经在很好的运行，不需要唤醒
        return NULL;
    }
    return t;
}