//
// Created by kingdo on 10/17/20.
//
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <test/test.h>
#include <tpool/tpool.h>
#include "sync/lock.h"
#include "instance/instance.h"

int main() {
    INIT_LOCK()
    resource res = DEFAULT_RESOURCE;
    func_register(taskF, "taskA", res, 1);
//    func_register(taskF, "taskB", res, 10);

    int count = 10000;
    F *f_A = get_func("taskA");
    for (int i = 0; i < count; ++i) {
        make_request(f_A, (void *) i);
    }
    printf("done\n");
    sleep(1000000);
    return 0;
}
