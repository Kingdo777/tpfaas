//
// Created by kingdo on 10/19/20.
//

#ifndef TPFAAS_SYNC_H
#define TPFAAS_SYNC_H

#include <thread_pool/tpool.h>
#include "function/function.h"


//随便一个整数就可以，这里就是我的幸运数字了
#define DEFAULT_FUTEX_WORD 7
#define WAKE_ONE 1
#define WAKE_ALL INT_MAX

void thread_sleep(int *futex_word);

void thread_wake_up_one(task *t);

void thread_wake_up_all(task *t);

#endif //TPFAAS_SYNC_H