//
// Created by kingdo on 10/17/20.
//
#include <unistd.h>
#include <tool/print.h>
#include <test/test.h>
#include "tool/tls.h"
#include "sync/lock.h"
#include "instance/instance.h"


unsigned long printf_fs;
unsigned long base;

int main() {
    GET_PRINTF_FS()
    INIT_LOCK()
    resource res = DEFAULT_RESOURCE;


    func_register(taskF, "taskA", res, 1);
//    func_register(taskF, "taskB", res, 10);
    F *f_A = get_func("taskA");
//    F *f_B = get_func("taskB");
    make_request(f_A, "HelloA\n", sizeof("HelloA\n"));
//    make_request(f_B, "Hello B\n", sizeof("Hello A\n"));

    sleep(1000000);
    return 0;
}
