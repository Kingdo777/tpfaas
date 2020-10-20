//
// Created by kingdo on 10/20/20.
//

#include "thread_pool/tpool.h"
#include "tool/tls.h"
#include "sync/sync.h"
#include <stdio.h>
#include <zconf.h>
#include "task.h"

LIST_HEAD(__task_list);

extern void gogo(void *new_stack, size_t stack_size);

void task_done() {
    TLS(t);
    {
        //TODO:这里应该是到队列里面去寻找，看看是否存在正在派对的任务，如果有的话那么拉过来执行
    }
    //最后才是阻塞
//    thread_sleep(&t->futex_word);
    printf("Work\n");
}

//线程刚创建时时执行的代码
int task_birth(void *arg) {
    TLS(t);
    if (t->futex_word == DEFAULT_FUTEX_WORD) {
        puts("child thread get tls true\n");
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
    exit(1);
}

void init_task(task *t) {
    t->futex_word = DEFAULT_FUTEX_WORD;
    MALLOC_DEFAULT_STACK(t->stack_top, t->stack_space);
    t->stack_size = DEFAULT_STACK_SIZE;
    t->next_func = NULL;
    INIT_LIST_HEAD(&t->task_list_node);
    list_add(&t->task_list_node, &__task_list);
}
