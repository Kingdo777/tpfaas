//
// Created by kingdo on 2020/10/27.
//

#ifndef TPFAAS_TEST_H
#define TPFAAS_TEST_H

#include <resource/resource.h>
#include <function/function.h>
#include <tool/tls.h>
#include <tool/print.h>
#include <string.h>
#include <task/task.h>
#include <sync/sync.h>
#include <unistd.h>
#include "tool/list_head.h"
#include "tool/stack.h"

extern list_head task_list_head;
extern list_head func_list_head;
extern pthread_mutex_t print_mutex;

void *taskF(void *arg) {
//    PRINTF("%s\n", (char *) arg)
//    puts(arg);
    return NULL;
}

static void function_test() {
    resource res = DEFAULT_RESOURCE;
    func_register(taskF, "taskA", res, 1);
    func_register(taskF, "taskB", res, 10);
    F *f;
    list_for_each_entry_prev(f, &func_list_head, func_list_list) {
        const char *func_name = f->name;
        printf("%s RUN:\n", func_name);
        FUN(func_name, "hello world\n")
    }
}


static void test_tls_gs() {
    //保留原来的FS
    SAVE_FS()

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
    RESTORE_FS()
}

static void test_tls() {
    //保留原来的FS
    SAVE_FS()

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
    RESTORE_FS()
}

_Noreturn static void call_all_task() {
    T *t;
    while (1) {
        list_for_each_entry(t, &task_list_head, task_list) {
            thread_wake_up_one(t);
        }
        sleep(3);
    }
}

static inline void stack_op_test() {
    void *s_top, *s_space;
    MALLOC_DEFAULT_STACK(s_top, s_space)
    printf("sp->%p\n", s_top);
    PUSH_INT(s_top, 7777)
    printf("sp->%p\n", s_top);
    PUSH_LONG(s_top, 888888)
    printf("sp->%p\n", s_top);
    PUSH_PTR(s_top, s_top)
    printf("sp->%p\n", s_top);

    int a;
    long b;
    void *s;
    POP_PTR(s_top, s)
    printf("sp->%p\n", s_top);
    POP_LONG(s_top, b)
    printf("sp->%p\n", s_top);
    POP_INT(s_top, a)
    printf("sp->%p\n", s_top);
    PRINT_INT(a);
    PRINT_LONG(b);
    printf("%p\n", s);
    exit(0);
}

#endif //TPFAAS_TEST_H
