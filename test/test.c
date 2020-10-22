#include "tls.h"
#include "task.h"
//
// Created by kingdo on 10/20/20.
//
void gogo(void *new_stack, size_t stack_size) {
    task *t;
    get_tls(&t);
    void *sp_old = t->stack_space;
    //  切换SP
    t->stack_space = new_stack;
    t->stack_top = new_stack + stack_size;
    //写入task_done
    t->stack_top -= sizeof(ulong);
    *(ulong *) (t->stack_top) = (ulong) task_done;
    //释放老栈
    free(sp_old);
    //   跳转
    void *pc = t->next_func->entry_addr;
}