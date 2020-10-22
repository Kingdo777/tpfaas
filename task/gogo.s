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
    jmp     %rdi
    .cfi_endproc

#void gogo_switch(void *new_stack);
	.text
	.globl	gogo_switch
	.type	gogo_switch, @function
gogo_switch:
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
	nop
    .cfi_def_cfa 7, 8
    ret
    .cfi_endproc