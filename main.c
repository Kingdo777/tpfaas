//
// Created by kingdo on 10/17/20.
//
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <linux/futex.h>
#include <syscall.h>
#include "tool/tls.h"
#include "tool/list_head.h"
#include "function/function.h"
#include "thread_pool/tpool.h"
#include "sync/sync.h"

extern list_head task_list__;

#define list_for_each_entry(pos, head, member)                \
    for (pos = list_first_entry(head, typeof(*pos), member);    \
         &pos->member != (head);                    \
         pos = list_next_entry(pos, member))

void *taskA(void *arg) {
    printf("AAAAAAAAAAA I am Task AAAAAAAAAAA\n");
    return NULL;
}

void *taskB(void *arg) {
    printf("BBBBBBBBBBB I am Task BBBBBBBBBBB\n");
    return NULL;
}

void *taskC(void *arg) {
    printf("CCCCCCCCCCC I am Task CCCCCCCCCCC\n");
    return NULL;
}


static void function_test() {
    func_register(taskA, "taskA");
    func_register(taskB, "taskB");
    func_register(taskC, "taskC");
    printf("taskA:%d\n", get_funcId_from_name("taskA"));
    printf("taskB:%d\n", get_funcId_from_name("taskB"));
    printf("taskB:%d\n", get_funcId_from_name("taskC"));
    FUN("taskA", NULL);
    FUN("taskB", NULL);
    FUN("taskC", NULL);
}

static void create_pool_test(int pool_size) {
    tpool_create_pool(pool_size);
    task *t1;
    list_for_each_entry(t1, &task_list__, task_list_node) {
        printf("%d:%d\n", t1->tgid, t1->futex_word);
    }
}

static void test_tls_gs() {
    //保留原来的FS
    unsigned long base;
    get_tls_gs(&base);

    int tls_gs_test = 18;
    set_tls_gs(&tls_gs_test);
    int *g;
    get_tls_gs(&g);
    if (tls_gs_test == *g) {
        printf("test tls_gs: TRUE\n");
    } else {
        printf("test tls_gs: FALSE\n");
    }

    //恢复原来的FS
    set_tls_gs((void *) base);
}

static void test_tls() {
    //保留原来的FS
    unsigned long base;
    get_tls(&base);

    //因为printf需要用到FS寄存器，因此set_tls后，printf函数将无法正常使用，只能打印字符串，不能打印数字
    char *tls_test = malloc(sizeof("hello tls"));
    strcpy(tls_test, "hello tls");
    set_tls((void *) tls_test);
    char *s_;
    get_tls(&s_);
    if (!strcmp(tls_test, s_)) {
        puts("test tls: TRUE");
    } else {
        puts("test tls: FALSE");
    }

    //恢复原来的FS
    set_tls((void *) base);
}

_Noreturn static void call_all_task() {
    task *t;
    while (1) {
        list_for_each_entry(t, &task_list__, task_list_node) {
            thread_wake_up_one(t);
        }
        sleep(1);
    }
}

int futex_word_test = DEFAULT_FUTEX_WORD;
int main() {
//    printf("123\n");
//    syscall(SYS_futex, futex_word_test, FUTEX_WAIT, DEFAULT_FUTEX_WORD);
//    puts("123\n");
//    return 0;
    function_test();

    create_pool_test(1);

    test_tls_gs();
    test_tls();
    printf("printf is Ok %d\n", 666);

//    call_all_task();
    sleep(1000000);
    return 0;
}
