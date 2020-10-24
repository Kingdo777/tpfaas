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
#include "tool/stack.h"
#include "task.h"

LIST_HEAD(task_list_head);

__pid_t task_pid_list__[TASK_MAX_COUNT];
int task_num__ = 0;

void get_instance() {
    TLS(t);
    find:
    if (t->deal_with != NULL && t->work_for != NULL)
        return;
    thread_sleep(&t->futex_word);
    goto find;
}

void task_done() {
    TLS(t);
    //清空执行Instance所保留的栈数据
    release_task_stack_when_sleep(t);
    //get_function如果找不到function，那么将一直睡眠，一旦返回说明已经找到
    get_instance();
    //找到的function被写到了task的next_function中
    gogo(t);
}

T *creat_new_task(R *r) {
    T *t = malloc(sizeof(T));
    if (t == NULL) {
        printf("malloc T space fault\n");
        return NULL;
    }
    if (init_task(t) && bind_os_thread(t)) {
        list_add(&r->task_idle_head, &t->task_idle_list);
        return t;
    }
    release_err_task(t);
    return NULL;
}


bool init_task(T *t) {
    t->futex_word = DEFAULT_FUTEX_WORD;
    t->work_for = NULL;
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