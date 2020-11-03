//
// Created by kingdo on 10/17/20.
//
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <test/test.h>
#include "sync/lock.h"
#include "instance/instance.h"

int main() {
    INIT_LOCK()
    resource res = DEFAULT_RESOURCE;
    func_register(taskF, "taskA", res, 1);
//    func_register(taskF, "taskB", res, 10);

    int count = 50000;
    F *f_A = get_func("taskA");
    for (int i = 0; i < count; ++i) {
        make_request(f_A, (void *) i);
    }
    R *r;
    take_an_entry_from_head(r, &res_list_head, res_list)
    printf("done\n");
    sleep(1000000);
    return 0;
}
