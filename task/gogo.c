//
// Created by kingdo on 2020/10/22.
//

#include "tool/stack.h"
#include "task.h"

extern void go_back_stack_and_jmp(void *stack_top, void *arg_stack, void *new_pc);

void gogo(T *t) {
    //切换堆栈以后,所有的,堆栈中的数据将全部无法使用,包括了局部变量等,因此,在gogo_switch之前定义的变量都无法使用,但是t是通过寄存器传递的,因此
    //还是可以用的,但是有的并非所有的编译器都是这样,一旦通过栈进行参数的传递,那么gogo_switch之后,参数t也无法使用
    PUSH_PTR(t->stack.stack_top, task_done)
    go_back_stack_and_jmp(t->stack.stack_top, t->deal_with->stack.stack_top, t->work_for->entry_addr);
}

