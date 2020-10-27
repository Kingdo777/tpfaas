//
// Created by kingdo on 2020/10/22.
//

#ifndef TPFAAS_STACK_H
#define TPFAAS_STACK_H

#include <stdlib.h>

void task_done();


#define PTR_SIZE (int)(sizeof(void*))
#define DEFAULT_STACK_SIZE 8*1024

#define MALLOC_STACK_SPACE(s_size) malloc(s_size)
#define MALLOC_DEFAULT_STACK_SPACE() MALLOC_STACK_SPACE(DEFAULT_STACK_SIZE)
#define MALLOC_STACK(s_top, s_space, s_size) do{s_space=malloc(s_size);\
    s_top=s_space+s_size;}while(0);
#define MALLOC_DEFAULT_STACK(s_top, s_space) MALLOC_STACK(s_top, s_space,DEFAULT_STACK_SIZE)
#define MALLOC_STACK_WITH_TASK_DONE(s_top, s_space, s_size) MALLOC_STACK(s_top, s_space, s_size) \
       PUSH_PTR(s_top,task_done)
#define MALLOC_DEFAULT_STACK_WITH_TASK_DONE(s_top, s_space) MALLOC_STACK_WITH_TASK_DONE(s_top, s_space, DEFAULT_STACK_SIZE)

#define PUSH_INT(s_top, a) PUSH(s_top,int,a)
#define POP_INT(s_top, a) POP(s_top,int,a)
#define PUSH_LONG(s_top, a) PUSH(s_top,long,a)
#define POP_LONG(s_top, a) POP(s_top,long,a)
#define PUSH_ADDR(s_top, a) PUSH(s_top,void *,a)
#define POP_ADDR(s_top, a) POP(s_top,void *,a)
#define PUSH_PTR PUSH_ADDR
#define POP_PTR POP_ADDR
#define PUSH(s_top, type, data) s_top-=sizeof(type);\
    *(type*)(s_top)=data;
#define POP(s_top, type, data)  data=*(type*)(s_top); \
    s_top+=sizeof(type);

//将s的栈结构copy到d中(不是数据copy)
#define COPY_STACK(s, d) do{ \
        d.stack_space=s.stack_space;  \
        d.stack_top=s.stack_top;      \
        d.stack_size=s.stack_size;    \
        }while(0);
#define COPY_STACK_DATA(s_top, data_addr, data_size) \
    s_top-=data_size;                                \
    memcpy(s_top,data_addr,data_size);
typedef struct {
    void *stack_space;//用于栈空间的手动释放
    void *stack_top;
    unsigned long stack_size;
} stack_struct;

#endif //TPFAAS_STACK_H
