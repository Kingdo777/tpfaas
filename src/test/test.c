
#include <sched.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <wait.h>
#include <ucontext.h>
#include <include/instance.h>


void *add() {
    return NULL;
}

struct ucontext_t ucontext1, ucontext2;
int current = 0;

void produce(int arg) {
    printf("%d", arg);
//    current++;
//    sleep(1);
//    setcontext(&ucontext2);
}

void consume() {
    printf("current value:%d\n", current);
    setcontext(&ucontext1);
}

int main_(int argc, const char *argv[]) {
//    getcontext(&ucontext1);
//    printf("123123\n");
//    sleep(1);
//    setcontext(&ucontext1);

    char stack1[STACK_DEFAULT_SIZE];
    char stack2[STACK_DEFAULT_SIZE];
    getcontext(&ucontext1);
    ucontext1.uc_link = NULL;
    ucontext1.uc_stack.ss_sp = stack1;
    ucontext1.uc_stack.ss_size = STACK_DEFAULT_SIZE;
    makecontext(&ucontext1, (void (*)(void)) produce, 1, 75475);

    getcontext(&ucontext2);
    ucontext2.uc_link = NULL;
    ucontext2.uc_stack.ss_sp = stack2;
    ucontext2.uc_stack.ss_size = STACK_DEFAULT_SIZE;
    makecontext(&ucontext2, (void (*)(void)) consume, 0);

    setcontext(&ucontext1);

    return 0;
}