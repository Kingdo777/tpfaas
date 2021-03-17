//
// Created by kingdo on 10/19/20.
//

#ifndef TPFAAS_SYNC_H
#define TPFAAS_SYNC_H

#include <limits.h>
#include "function.h"
#include "task.h"

#define DEFAULT_FUTEX_WORD 7
#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE);} while (0)

void thread_sleep(T *t);

void thread_wake_up_one(T *t);


#endif //TPFAAS_SYNC_H
