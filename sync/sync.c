//
// Created by kingdo on 10/19/20.
//

#include "sync.h"
#include <linux/futex.h>
#include <unistd.h>
#include <syscall.h>

void thread_sleep(int *futex_word) {
    syscall(SYS_futex, futex_word, FUTEX_WAIT, DEFAULT_FUTEX_WORD);
}

void thread_wake_up_one(task *t) {
    syscall(SYS_futex, &t->futex_word, FUTEX_WAKE, WAKE_ONE);
}

void thread_wake_up_all(task *t) {
    syscall(SYS_futex, &t->futex_word, FUTEX_WAKE, WAKE_ALL);
}
