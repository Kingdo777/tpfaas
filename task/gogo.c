//
// Created by kingdo on 2020/10/22.
//

#include <stdio.h>
#include "function/function.h"
#include "tool/stack.h"
#include "tool/print.h"
#include "task.h"
#include "instance/instance.h"

void gogo(T *t) {

//    malloc_instance_stack_when_create(t->deal_with, "Hello clone\n", sizeof("Hello clone\n"));

    void *old_stack_space = t->stack.stack_space;
    COPY_STACK(t->deal_with->stack, t->stack)
    //切换堆栈以后,所有的,堆栈中的数据将全部无法使用,包括了局部变量等,因此,在gogo_switch之前定义的变量都无法使用,但是t是通过寄存器传递的,因此
    //还是可以用的,但是有的并非所有的编译器都是这样,一旦通过栈进行参数的传递,那么gogo_switch之后,参数t也无法使用
    malloc(1);
    gogo_switch_new_free_old_and_jmp(t->stack.stack_top, old_stack_space, t->work_for->entry_addr);
}

