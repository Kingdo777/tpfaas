#define _GNU_SOURCE

#include <sched.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <wait.h>

void *add() {
    return NULL;
}

int main_test() {
    write(1, "start\n", 6);
    void *tls = malloc(100);
    pid_t pid = clone((void *) add, malloc(10000),
                      CLONE_VM | CLONE_SETTLS,
                      "123", NULL, &tls, NULL);
    waitpid(pid, NULL, __WCLONE);
    return 0;
}