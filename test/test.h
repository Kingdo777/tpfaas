//
// Created by kingdo on 2020/10/27.
//

#ifndef TPFAAS_TEST_H
#define TPFAAS_TEST_H

#include <resource/resource.h>
#include <function/function.h>
#include <string.h>
#include <task/task.h>
#include <sync/sync.h>
#include <unistd.h>
#include <tool/time_k.h>
#include "tool/list_head.h"

extern list_head task_list_head;
extern list_head func_list_head;

void *taskF(void *arg) {
    usleep(100 * MS_US);
//    printf("run %d\n", (int) (arg));
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

#endif //TPFAAS_TEST_H
