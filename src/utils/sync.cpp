//
// Created by kingdo on 10/19/20.
//
#include <semaphore.h>
#include "utils/sync.h"


void thread_sleep(T *t) {
    sem_wait(&t->getSemWord());
}

void thread_wake_up_one(T *t) {
    sem_post(&t->getSemWord());
}


//void thread_sleep(T *t) {
//    pthread_mutex_lock(&t->t_mutex);
//    pthread_cond_wait(&t->t_cont, &t->t_mutex);
//    pthread_mutex_unlock(&t->t_mutex);
//}
//
//void thread_wake_up_one(T *t) {
//    pthread_mutex_lock(&t->t_mutex);
//    pthread_cond_signal(&t->t_cont);
//    pthread_mutex_unlock(&t->t_mutex);
//}


//static long futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
//    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
//}
//
//void thread_sleep(T *t) {
//    long s = futex(&t->futex_word, FUTEX_WAIT, DEFAULT_FUTEX_WORD, NULL, NULL, 0);
//    if (s == -1 && errno != EAGAIN) {
//        errExit("futex-FUTEX_WAIT");
//    }
//}
//
//void thread_wake_up_one(T *t) {
//    long s = futex(&t->futex_word, FUTEX_WAKE, 1, NULL, NULL, 0);
//    if (s != 1)
//        errExit("futex-FUTEX_WAKE");
//}