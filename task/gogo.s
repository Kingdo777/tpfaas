	.file	"gogo.s"
#void gogo_jmp(void *new_pc);
	.text
	.globl	gogo_jmp
	.type	gogo_jmp, @function
gogo_jmp:
.LFB8:
	.cfi_startproc
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	nop
    .cfi_def_cfa 7, 8
    # 跳转到用户线程
    movq    %rdi,   %rax
    movq    %rsp,   %rdi
    addq    $8,     %rdi
    jmp     %rax
    .cfi_endproc

#void gogo_switch_new_free_old(void *new_stack, void *old_stack);
	.text
	.globl	gogo_switch_new_free_old
	.type	gogo_switch_new_free_old, @function
gogo_switch_new_free_old:
.LFB9:
	.cfi_startproc
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	# 保存原来的PC，保证正常返回
	popq    %rax
	# 切换栈空间
	movq    %rdi,%rsp
	# 恢复原来的PC
	pushq   %rax
	# 删除原来的栈空间
    movq    %rsi,%rdi
    call	free@PLT
	nop
    .cfi_def_cfa 7, 8
    ret
    .cfi_endproc

#void gogo_switch_new_free_old_and_jmp(void *new_stack, void *old_stack, void *new_pc);
	.text
	.globl	gogo_switch_new_free_old_and_jmp
	.type	gogo_switch_new_free_old_and_jmp, @function
gogo_switch_new_free_old_and_jmp:
.LFB10:
	.cfi_startproc
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	# 切换栈空间
	movq    %rdi,%rsp
	pushq   %rdx

	# 删除原来的栈空间
    movq    %rsi,%rdi
    call	free@PLT

    #跳转
    popq    %rax
    movq    %rsp,   %rdi
    addq    $8,     %rdi
    jmp     %rax
	nop
    .cfi_def_cfa 7, 8
    .cfi_endproc

#void go_back_stack_and_jmp(void *stack_top, void *arg_stack, void *new_pc);
	.text
	.globl	go_back_stack_and_jmp
	.type	go_back_stack_and_jmp, @function
go_back_stack_and_jmp:
.LFB11:
	.cfi_startproc
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	# 切换栈空间
	movq    %rdi,%rsp
	# 导入参数栈
    movq    %rsi,%rdi
    # 跳转
    jmp     %rdx
	nop
    .cfi_def_cfa 7, 8
    .cfi_endproc