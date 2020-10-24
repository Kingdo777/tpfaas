//
// Created by kingdo on 2020/10/22.
//

#include "function/function.h"
#include "tool/stack.h"
#include "task.h"

void gogo(T *t) {
    void *old_stack_space = t->stack.stack_space;
    COPY_STACK(t->deal_with->stack, t->stack);
    gogo_switch(t->stack.stack_top);
    free(old_stack_space);
    gogo_jmp(t->work_for->entry_addr);
}

