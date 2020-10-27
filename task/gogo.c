//
// Created by kingdo on 2020/10/22.
//

#include <stdio.h>
#include "function/function.h"
#include "tool/stack.h"
#include "tool/print.h"
#include "task.h"

void gogo(T *t) {
    stack_struct stack;
    MALLOC_DEFAULT_STACK(stack.stack_top, stack.stack_space);
    COPY_STACK_DATA(stack.stack_top, "Hello A\n", sizeof("Hello A\n"))
    PUSH_PTR(stack.stack_top, task_done)
    stack.stack_size = DEFAULT_STACK_SIZE;

    void *old_stack_space = t->stack.stack_space;
    COPY_STACK(stack, t->stack)
    //切换堆栈以后,所有的,堆栈中的数据将全部无法使用,包括了局部变量等,因此,在gogo_switch之前定义的变量都无法使用,但是t是通过寄存器传递的,因此
    //还是可以用的,但是有的并非所有的编译器都是这样,一旦通过栈进行参数的传递,那么gogo_switch之后,参数t也无法使用
    gogo_switch_new_free_old_and_jmp(t->stack.stack_top, old_stack_space, t->work_for->entry_addr);


//    void *old_stack_space = t->stack.stack_space;
//    COPY_STACK(t->deal_with->stack, t->stack)
//    //切换堆栈以后,所有的,堆栈中的数据将全部无法使用,包括了局部变量等,因此,在gogo_switch之前定义的变量都无法使用,但是t是通过寄存器传递的,因此
//    //还是可以用的,但是有的并非所有的编译器都是这样,一旦通过栈进行参数的传递,那么gogo_switch之后,参数t也无法使用
//    gogo_switch_new_free_old_and_jmp(t->stack.stack_top, old_stack_space, t->work_for->entry_addr);
}

