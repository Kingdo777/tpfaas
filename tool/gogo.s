#void gogo(void *new_stack, size_t stack_size)
	.file	"gogo.c"
	.text
	.globl	gogo
	.type	gogo, @function
gogo:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	# 分配栈空间
	subq	$48, %rsp
	# 将参数放入栈中
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
    # task *t;
    # get_tls(&t);
	xorl	%eax, %eax
	leaq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	get_tls@PLT
	# void *sp_old = t->stack_space;
	movq	-32(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, -24(%rbp)
	# t->stack_space = new_stack;
    # t->stack_top = new_stack + stack_size;
	movq	-32(%rbp), %rax
	movq	-40(%rbp), %rdx
	movq	%rdx, 8(%rax)
	movq	-32(%rbp), %rax
	movq	-40(%rbp), %rcx
	movq	-48(%rbp), %rdx
	addq	%rcx, %rdx
	movq	%rdx, 16(%rax)
	# 切换SP
	movq    %rdx, %rsp
	movq	task_done@GOTPCREL(%rip), %rax
	pushq   %rax
	# 释放老栈
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	# TODO:这里的free有优化的空间，因为可以仅仅取消物理页面和逻辑页面的映射，这样就以加快内存的分配
	call	free@PLT
	# void *pc = t->next_func;
	movq	-32(%rbp), %rax
	movq	32(%rax), %rax
	movq	%rax, -16(%rbp)
	nop
    # 跳转到用户线程
    jmp     %rax


#gogo函数栈帧
#+----------------------+  高地址
#|         %rip         |
#+----------------------+
#|         %rbp         |
#+----------------------+ <--- rbp
#|                      |
#+----------------------+ <--- -8(rbp)
#|          pc          |
#+----------------------+ <--- -16(rbp)
#|        sp_old        |
#+----------------------+ <--- -24(rbp)
#|           t          |
#+----------------------+ <--- -32(rbp)
#|       stack_size     |
#+----------------------+ <--- -40(rbp)
#|       new_stack      |
#+----------------------+ <--- -48(rbp)   <---rsp

#task的结构：               低地址
#+----------------------+  <--- task(0)
#|         tgid         |
#+----------------------+  <--- task(4)
#|      futex_word      |
#+----------------------+  <--- task(8)
#|      stack_space     |
#+----------------------+  <--- task(16)
#|       stack_top      |
#+----------------------+  <--- task(24)
#|       stack_size     |
#+----------------------+  <--- task(32)
#|       next_func      |
#+----------------------+  <--- task(...)
#|          ...         |
#+----------------------+