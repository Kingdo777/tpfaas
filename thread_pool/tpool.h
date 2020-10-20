//
// Created by kingdo on 10/18/20.
//

#ifndef TPFAAS_TPOOL_H
#define TPFAAS_TPOOL_H

#include <stdlib.h>
#include <tool/list_head.h>

#define TASK_MAX_COUNT 10000
#define DEFAULT_STACK_SIZE 8*1024

#define MALLOC_STACK(s) (malloc(s) + s -1)
#define MALLOC_DEFAULT_STACK() MALLOC_STACK(DEFAULT_STACK_SIZE)

#define ADD_2_TASK_LIST(t)

typedef u_int64_t u64;

typedef struct task {
    //pid
    __pid_t tgid;
    //futex word
    int futex_word;
    list_head task_list_node;
} task;

typedef struct clone_args {
    u64 flags;        /* Flags bit mask */
    u64 pidfd;        /* Where to store PID file descriptor
                                    (pid_t *) */
    u64 child_tid;    /* Where to store child TID,
                                    in child's memory (pid_t *) */
    u64 parent_tid;   /* Where to store child TID,
                                    in parent's memory (int *) */
    u64 exit_signal;  /* Signal to deliver to parent on
                                    child termination */
    u64 stack;        /* Pointer to lowest byte of stack */
    u64 stack_size;   /* Size of stack */
    u64 tls;          /* Location of new TLS */
    u64 set_tid;      /* Pointer to a pid_t array
                                    (since Linux 5.5) */
    u64 set_tid_size; /* Number of elements in set_tid
                                    (since Linux 5.5) */
    u64 cgroup;       /* File descriptor for target cgroup
                                    of child (since Linux 5.7) */
} clone_args;

void tpool_create_pool(int pool_size);

#endif //TPFAAS_TPOOL_H
