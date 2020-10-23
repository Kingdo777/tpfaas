//
// Created by kingdo on 10/17/20.
//
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <tool/print.h>
#include "tool/tls.h"
#include "tool/list_head.h"
#include "function/function.h"
#include "thread_pool/tpool.h"
#include "sync/sync.h"

extern list_head task_list__;

unsigned long printf_fs;
unsigned long base;

void *taskA(void *arg) {
    PRINTF("%s I am Task %s\n", "AAAAAAAAAAA", "AAAAAAAAAAA");
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
    FUN("taskA", NULL)
    FUN("taskB", NULL)
    FUN("taskC", NULL)
}

static void create_pool_test(int pool_size) {
    tpool_create_pool(pool_size);
    T *t1;
    list_for_each_entry(t1, &task_list__, task_list) {
        printf("%d:%d\n", t1->tgid, t1->futex_word);
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
        list_for_each_entry(t, &task_list__, task_list) {
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


int main() {
    GET_PRINTF_FS()
//    test();
    function_test();

//    create_pool_test(1);
//
//    test_tls_gs();
//    test_tls();
//    printf("printf is Ok %d\n", 666);

//    call_all_task();
    add_task_2_pool_with_func(get_func("taskA"));

    sleep(1000000);
    return 0;
}
