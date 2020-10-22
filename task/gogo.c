//
// Created by kingdo on 2020/10/22.
//

#include <function/function.h>
#include "task.h"

extern void gogo_switch(void *new_stack);
extern void gogo_jmp(void *new_pc);

void gogo(task *t) {
    void *old_stack_space = t->stack.stack_space;
    gogo_switch(t->next_func->stack.stack_top);
    free(old_stack_space);
    gogo_jmp(t->next_func->entry_addr);
}

