//
// Created by kingdo on 10/19/20.
//

#ifndef TPFAAS_SYNC_H
#define TPFAAS_SYNC_H

#include <limits.h>
#include "function/function.h"
#include "task/task.h"

void thread_sleep(T *t);

void thread_wake_up_one(T *t);


#endif //TPFAAS_SYNC_H
