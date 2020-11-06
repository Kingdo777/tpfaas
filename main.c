//
// Created by kingdo on 10/17/20.
//

#include <stdio.h>
#include <unistd.h>
#include <test/test.h>
#include "tool/print.h"
#include "sync/lock.h"
#include "instance/instance.h"

int main() {
    INIT_LOCK()
    resource res = DEFAULT_RESOURCE;
    func_register(taskF, "taskA", res, 100);
    int count = 500000;
    F *f_A = get_func("taskA");
    for (int i = 1; i <= count; ++i) {
        make_request(f_A, (void *) i);
    }
    printf("done\n");
    for (int i = 0; i < 100; ++i) {
        print_info();
        sleep(1);
    }
    sleep(1000000);
    return 0;
}
