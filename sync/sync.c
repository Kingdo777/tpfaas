//
// Created by kingdo on 10/19/20.
//
#define _GNU_SOURCE

#include "sync.h"
#include <linux/futex.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/time.h>


static long futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}


void thread_sleep(int *futex_word) {
    long s = futex(futex_word, FUTEX_WAIT, DEFAULT_FUTEX_WORD, NULL, NULL, 0);
    if (s == -1 && errno != EAGAIN) {
        errExit("futex-FUTEX_WAIT");
    }
}

void thread_wake_up_one(task *t) {

    long s = futex(&t->futex_word, FUTEX_WAKE, WAKE_ONE, NULL, NULL, 0);
    if (s == -1)
        errExit("futex-FUTEX_WAKE");
}

void thread_wake_up_all(task *t) {
    long s = futex(&t->futex_word, FUTEX_WAKE, WAKE_ONE, NULL, NULL, 0);
    if (s == -1)
        errExit("futex-FUTEX_WAKE");
}
