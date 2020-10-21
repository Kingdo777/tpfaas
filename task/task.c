//
// Created by kingdo on 10/20/20.
//

#include "thread_pool/tpool.h"
#include "tool/tls.h"
#include "sync/sync.h"
#include <stdio.h>
#include <unistd.h>
#include "task.h"

LIST_HEAD(task_list__);


extern void gogo(void *new_stack, size_t stack_size);



void get_function() {
    TLS(t);
    func f;
//    find:
    if (t->next_func != NULL) {
        return;
    }
    //TODO 这里是在一个队列里寻找任务
//    puts("no function to run\n");
    thread_sleep(&t->futex_word);
//    goto find;
}

void task_done() {
    //get_function如果找不到function，那么将一直睡眠，一旦返回说明已经找到
    get_function();
    //找到的function被写到了task的next_function中
//    gogo(MALLOC_DEFAULT_STACK_SPACE(), DEFAULT_STACK_SIZE);
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
    }
    task_done();
//    for (int i = 0; i < 5; i++) {
//        puts("child ready to work\n");
//        sleep(1);
//    }
//    thread_sleep(&t->futex_word);
//    for (int i = 0; i < 5; i++) {
//        puts("child back to work\n");
//        sleep(1);
//    }
    //never arrive
    printf("exit error\n");
    // 线程是不能执行exit操作的，因为会终止所有的线程，只要是指定了clone的CLONE_VM参数,就会终止所有的线程
//     exit(1);
}

void init_task(task *t) {
    t->futex_word = DEFAULT_FUTEX_WORD;
    MALLOC_DEFAULT_STACK(t->stack_top, t->stack_space);
    t->stack_size = DEFAULT_STACK_SIZE;
    t->next_func = NULL;
    INIT_LIST_HEAD(&t->task_list_node);
    list_add(&t->task_list_node, &task_list__);
}
