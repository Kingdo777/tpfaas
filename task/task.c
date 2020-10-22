//
// Created by kingdo on 10/20/20.
//
#define _GNU_SOURCE

#include <sched.h>
#include "thread_pool/tpool.h"
#include "tool/tls.h"
#include "sync/sync.h"
#include <stdio.h>
#include <unistd.h>
#include "tool/stack.h"
#include "task.h"

LIST_HEAD(task_list__);

void get_function() {
    TLS(t);
    func f;
    find:
    if (t->next_func != NULL) {
        return;
    }
    //TODO 这里是在一个队列里寻找任务
    puts("no function to run\n");
    thread_sleep(&t->futex_word);
    goto find;
}

void task_done() {
    TLS(t);
    t->next_func = NULL;
    //get_function如果找不到function，那么将一直睡眠，一旦返回说明已经找到
    get_function();
    //找到的function被写到了task的next_function中
    gogo(t);
}

//线程刚创建时执行的代码
int task_birth(void *arg) {
    TLS(t);
    if (t->futex_word == DEFAULT_FUTEX_WORD) {
        puts("child thread get tls true\n");
    } else {
        puts("child thread get tls wrong\n");
    }
    if (t->next_func != NULL) {
        //TODO 进入用户函数
        gogo(t);
    }
    task_done();
    //never arrive
    printf("exit error\n");
    // 线程是不能执行exit操作的，因为会终止所有的线程，只要是指定了clone的CLONE_VM参数,就会终止所有的线程
    // exit(1);
    return 0;
}

task *creat_task(func *f) {
    task *t = malloc(sizeof(task));
    init_task(t, f);
    /**
         * 这里仅仅设置了CLONE_VM参数而没有设置CLONE_THREAD flag是一个重大的决定，这意味着，我们clone的线程和posix定义的线程不同
         * 可以认为我们定义的是一个共享内存地址空间的进程，这样clone的每个进程依然是一个线程组，但是exec、exit仍然还是一个危险的操作
         *
         * CLONE_SETTLS 使用的也是arch_prctl系统调用，利用FS实现线程存储
         *
         * SIGCHLD，子进程退出时，向父进程发送的signal，如果不发送这个信号，那么是没办法被wait()获取的，只能使用waitpid + __WALL/__WCLONE等参数
         *
        */
    t->tgid = clone(task_birth, t->stack.stack_top,
                    CLONE_VM |
                    CLONE_SETTLS |
                    CLONE_CHILD_SETTID |
                    CLONE_PARENT_SETTID,
                    NULL, &task_pid_list__[task_num__++], t, &t->tgid);
    if (t->tgid == -1) {
        printf("clone wrong\n");
    }
    return t;
}

void init_task(task *t, func *f) {
    t->futex_word = DEFAULT_FUTEX_WORD;
    MALLOC_DEFAULT_STACK(t->stack.stack_top, t->stack.stack_space);
    t->stack.stack_size = DEFAULT_STACK_SIZE;
    t->next_func = f;
    INIT_LIST_HEAD(&t->task_list_node);
    list_add(&t->task_list_node, &task_list__);
}