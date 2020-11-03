//
// Created by kingdo on 10/19/20.
//
#define _GNU_SOURCE

#include "sync.h"


void thread_sleep(T *t) {
    pthread_mutex_lock(&t->t_mutex);
    pthread_cond_wait(&t->t_cont, &t->t_mutex);
}

void thread_wake_up_one(T *t) {
    pthread_cond_signal(&t->t_cont);
    pthread_mutex_unlock(&t->t_mutex);
}
