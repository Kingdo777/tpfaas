! [0x00007f08588c1a00..0x00007f08588c1cff]:
	mov    $0xffffffffffffffea,%rax                        ! 0x00007f08588c1a00
	test   %rdi,%rdi                                       ! 0x00007f08588c1a07
	je     0x7f08588c1a49  # <clone+73>                    ! 0x00007f08588c1a0a
	test   %rsi,%rsi                                       ! 0x00007f08588c1a0c
	je     0x7f08588c1a49  # <clone+73>                    ! 0x00007f08588c1a0f
	sub    $0x10,%rsi                                      ! 0x00007f08588c1a11
	mov    %rcx,0x8(%rsi)                                  ! 0x00007f08588c1a15
	mov    %rdi,(%rsi)                                     ! 0x00007f08588c1a19
	mov    %rdx,%rdi                                       ! 0x00007f08588c1a1c
	mov    %r8,%rdx                                        ! 0x00007f08588c1a1f
	mov    %r9,%r8                                         ! 0x00007f08588c1a22
	mov    0x8(%rsp),%r10                                  ! 0x00007f08588c1a25
	mov    $0x38,%eax                                      ! 0x00007f08588c1a2a
	syscall                                                ! 0x00007f08588c1a2f
	test   %rax,%rax                                       ! 0x00007f08588c1a31
	jl     0x7f08588c1a49  # <clone+73>                    ! 0x00007f08588c1a34
	je     0x7f08588c1a39  # <clone+57>                    ! 0x00007f08588c1a36
	retq                                                   ! 0x00007f08588c1a38
	xor    %ebp,%ebp                                       ! 0x00007f08588c1a39
	pop    %rax                                            ! 0x00007f08588c1a3b
	pop    %rdi                                            ! 0x00007f08588c1a3c
	callq  *%rax                                           ! 0x00007f08588c1a3d
	mov    %rax,%rdi                                       ! 0x00007f08588c1a3f
	mov    $0x3c,%eax                                      ! 0x00007f08588c1a42
	syscall                                                ! 0x00007f08588c1a47
	mov    0x2c9418(%rip),%rcx        # 0x7f0858b8ae68     ! 0x00007f08588c1a49
	neg    %eax                                            ! 0x00007f08588c1a50
	mov    %eax,%fs:(%rcx)                                 ! 0x00007f08588c1a52
	or     $0xffffffffffffffff,%rax                        ! 0x00007f08588c1a55
	retq                                                   ! 0x00007f08588c1a59
	nopw   0x0(%rax,%rax,1)                                ! 0x00007f08588c1a5a
	xor    %esi,%esi                                       ! 0x00007f08588c1a60
	jmpq   0x7f08588c1a70  # <umount2>                     ! 0x00007f08588c1a62
	nopw   0x0(%rax,%rax,1)                                ! 0x00007f08588c1a67
	mov    $0xa6,%eax                                      ! 0x00007f08588c1a70
	syscall                                                ! 0x00007f08588c1a75
	cmp    $0xfffffffffffff001,%rax                        ! 0x00007f08588c1a77
	jae    0x7f08588c1a80  # <umount2+16>                  ! 0x00007f08588c1a7d
	retq                                                   ! 0x00007f08588c1a7f
	mov    0x2c93e1(%rip),%rcx        # 0x7f0858b8ae68     ! 0x00007f08588c1a80
	neg    %eax                                            ! 0x00007f08588c1a87
	mov    %eax,%fs:(%rcx)                                 ! 0x00007f08588c1a89
	or     $0xffffffffffffffff,%rax                        ! 0x00007f08588c1a8c
	retq                                                   ! 0x00007f08588c1a90
	nopw   %cs:0x0(%rax,%rax,1)                            ! 0x00007f08588c1a91
	nopl   0x0(%rax,%rax,1)                                ! 0x00007f08588c1a9b
	mov    $0xbb,%eax                                      ! 0x00007f08588c1aa0
	syscall                                                ! 0x00007f08588c1aa5
	cmp    $0xfffffffffffff001,%rax                        ! 0x00007f08588c1aa7
	jae    0x7f08588c1ab0  # <readahead+16>                ! 0x00007f08588c1aad
	retq                                                   ! 0x00007f08588c1aaf
	mov    0x2c93b1(%rip),%rcx        # 0x7f0858b8ae68     ! 0x00007f08588c1ab0
	neg    %eax                                            ! 0x00007f08588c1ab7
	mov    %eax,%fs:(%rcx)                                 ! 0x00007f08588c1ab9
	or     $0xffffffffffffffff,%rax                        ! 0x00007f08588c1abc
	retq                                                   ! 0x00007f08588c1ac0
	nopw   %cs:0x0(%rax,%rax,1)                            ! 0x00007f08588c1ac1
	nopl   0x0(%rax,%rax,1)                                ! 0x00007f08588c1acb
	mov    $0x7a,%eax                                      ! 0x00007f08588c1ad0
	syscall                                                ! 0x00007f08588c1ad5
	cmp    $0xfffffffffffff001,%rax                        ! 0x00007f08588c1ad7
	jae    0x7f08588c1ae0  # <setfsuid+16>                 ! 0x00007f08588c1add
	retq                                                   ! 0x00007f08588c1adf
	mov    0x2c9381(%rip),%rcx        # 0x7f0858b8ae68     ! 0x00007f08588c1ae0
	neg    %eax                                            ! 0x00007f08588c1ae7
	mov    %eax,%fs:(%rcx)                                 ! 0x00007f08588c1ae9
	or     $0xffffffffffffffff,%rax                        ! 0x00007f08588c1aec
	retq                                                   ! 0x00007f08588c1af0
	nopw   %cs:0x0(%rax,%rax,1)                            ! 0x00007f08588c1af1
	nopl   0x0(%rax,%rax,1)                                ! 0x00007f08588c1afb
	mov    $0x7b,%eax                                      ! 0x00007f08588c1b00
	syscall                                                ! 0x00007f08588c1b05
	cmp    $0xfffffffffffff001,%rax                        ! 0x00007f08588c1b07
	jae    0x7f08588c1b10  # <setfsgid+16>                 ! 0x00007f08588c1b0d
	retq                                                   ! 0x00007f08588c1b0f
	mov    0x2c9351(%rip),%rcx        # 0x7f0858b8ae68     ! 0x00007f08588c1b10
	neg    %eax                                            ! 0x00007f08588c1b17
	mov    %eax,%fs:(%rcx)                                 ! 0x00007f08588c1b19
	or     $0xffffffffffffffff,%rax                        ! 0x00007f08588c1b1c
	retq                                                   ! 0x00007f08588c1b20
	nopw   %cs:0x0(%rax,%rax,1)                            ! 0x00007f08588c1b21
	nopl   0x0(%rax,%rax,1)                                ! 0x00007f08588c1b2b
	lea    0x2cedc1(%rip),%rax        # 0x7f0858b908f8 <__libc_multiple_threads> ! 0x00007f08588c1b30
	mov    %ecx,%r10d                                      ! 0x00007f08588c1b37
	mov    (%rax),%eax                                     ! 0x00007f08588c1b3a
	test   %eax,%eax                                       ! 0x00007f08588c1b3c
	jne    0x7f08588c1b60  # <__GI_epoll_pwait+48>         ! 0x00007f08588c1b3e
	mov    $0x8,%r9d                                       ! 0x00007f08588c1b40
	mov    $0x119,%eax                                     ! 0x00007f08588c1b46
	syscall                                                ! 0x00007f08588c1b4b
	cmp    $0xfffffffffffff000,%rax                        ! 0x00007f08588c1b4d
	ja     0x7f08588c1bc8  # <__GI_epoll_pwait+152>        ! 0x00007f08588c1b53
	repz retq                                              ! 0x00007f08588c1b55
	nopw   0x0(%rax,%rax,1)                                ! 0x00007f08588c1b57
	push   %r15                                            ! 0x00007f08588c1b60
	push   %r14                                            ! 0x00007f08588c1b62
	mov    %r8,%r15                                        ! 0x00007f08588c1b64
	push   %r13                                            ! 0x00007f08588c1b67
	push   %r12                                            ! 0x00007f08588c1b69
	mov    %ecx,%r14d                                      ! 0x00007f08588c1b6b
	push   %rbp                                            ! 0x00007f08588c1b6e
	push   %rbx                                            ! 0x00007f08588c1b6f
	mov    %edx,%r13d                                      ! 0x00007f08588c1b70
	mov    %rsi,%r12                                       ! 0x00007f08588c1b73
	mov    %edi,%ebx                                       ! 0x00007f08588c1b76
	sub    $0x18,%rsp                                      ! 0x00007f08588c1b78
	callq  0x7f08588d0890  # <__libc_enable_asynccancel>   ! 0x00007f08588c1b7c
	mov    $0x8,%r9d                                       ! 0x00007f08588c1b81
	mov    %eax,%ebp                                       ! 0x00007f08588c1b87
	mov    %r15,%r8                                        ! 0x00007f08588c1b89
	mov    %r14d,%r10d                                     ! 0x00007f08588c1b8c
	mov    %r13d,%edx                                      ! 0x00007f08588c1b8f
	mov    %r12,%rsi                                       ! 0x00007f08588c1b92
	mov    %ebx,%edi                                       ! 0x00007f08588c1b95
	mov    $0x119,%eax                                     ! 0x00007f08588c1b97
	syscall                                                ! 0x00007f08588c1b9c
	cmp    $0xfffffffffffff000,%rax                        ! 0x00007f08588c1b9e
	ja     0x7f08588c1bda  # <__GI_epoll_pwait+170>        ! 0x00007f08588c1ba4
	mov    %ebp,%edi                                       ! 0x00007f08588c1ba6
	mov    %eax,0xc(%rsp)                                  ! 0x00007f08588c1ba8
	callq  0x7f08588d08f0  # <__libc_disable_asynccancel>  ! 0x00007f08588c1bac
	mov    0xc(%rsp),%eax                                  ! 0x00007f08588c1bb1
	add    $0x18,%rsp                                      ! 0x00007f08588c1bb5
	pop    %rbx                                            ! 0x00007f08588c1bb9
	pop    %rbp                                            ! 0x00007f08588c1bba
	pop    %r12                                            ! 0x00007f08588c1bbb
	pop    %r13                                            ! 0x00007f08588c1bbd
	pop    %r14                                            ! 0x00007f08588c1bbf
	pop    %r15                                            ! 0x00007f08588c1bc1
	retq                                                   ! 0x00007f08588c1bc3
	nopl   0x0(%rax)                                       ! 0x00007f08588c1bc4
	mov    0x2c9299(%rip),%rdx        # 0x7f0858b8ae68     ! 0x00007f08588c1bc8
	neg    %eax                                            ! 0x00007f08588c1bcf
	mov    %eax,%fs:(%rdx)                                 ! 0x00007f08588c1bd1
	mov    $0xffffffff,%eax                                ! 0x00007f08588c1bd4
	retq                                                   ! 0x00007f08588c1bd9
	mov    0x2c9287(%rip),%rdx        # 0x7f0858b8ae68     ! 0x00007f08588c1bda
	neg    %eax                                            ! 0x00007f08588c1be1
	mov    %eax,%fs:(%rdx)                                 ! 0x00007f08588c1be3
	mov    $0xffffffff,%eax                                ! 0x00007f08588c1be6
	jmp    0x7f08588c1ba6  # <__GI_epoll_pwait+118>        ! 0x00007f08588c1beb
	nopl   (%rax)                                          ! 0x00007f08588c1bed
	mov    %edx,%r10d                                      ! 0x00007f08588c1bf0
	mov    $0x121,%eax                                     ! 0x00007f08588c1bf3
	mov    $0x8,%edx                                       ! 0x00007f08588c1bf8
	syscall                                                ! 0x00007f08588c1bfd
	cmp    $0xfffffffffffff000,%rax                        ! 0x00007f08588c1bff
	ja     0x7f08588c1c10  # <signalfd+32>                 ! 0x00007f08588c1c05
	repz retq                                              ! 0x00007f08588c1c07
	nopl   0x0(%rax)                                       ! 0x00007f08588c1c09
	mov    0x2c9251(%rip),%rdx        # 0x7f0858b8ae68     ! 0x00007f08588c1c10
	neg    %eax                                            ! 0x00007f08588c1c17
	mov    %eax,%fs:(%rdx)                                 ! 0x00007f08588c1c19
	mov    $0xffffffff,%eax                                ! 0x00007f08588c1c1c
	retq                                                   ! 0x00007f08588c1c21
	nopw   %cs:0x0(%rax,%rax,1)                            ! 0x00007f08588c1c22
	nopl   0x0(%rax)                                       ! 0x00007f08588c1c2c
	mov    $0x122,%eax                                     ! 0x00007f08588c1c30
	syscall                                                ! 0x00007f08588c1c35
	cmp    $0xfffffffffffff001,%rax                        ! 0x00007f08588c1c37
	jae    0x7f08588c1c40  # <eventfd+16>                  ! 0x00007f08588c1c3d
	retq                                                   ! 0x00007f08588c1c3f
	mov    0x2c9221(%rip),%rcx        # 0x7f0858b8ae68     ! 0x00007f08588c1c40
	neg    %eax                                            ! 0x00007f08588c1c47
	mov    %eax,%fs:(%rcx)                                 ! 0x00007f08588c1c49
	or     $0xffffffffffffffff,%rax                        ! 0x00007f08588c1c4c
	retq                                                   ! 0x00007f08588c1c50
	nopw   %cs:0x0(%rax,%rax,1)                            ! 0x00007f08588c1c51
	nopl   0x0(%rax,%rax,1)                                ! 0x00007f08588c1c5b
	sub    $0x8,%rsp                                       ! 0x00007f08588c1c60
	mov    $0x8,%edx                                       ! 0x00007f08588c1c64
	callq  0x7f08588b0180  # <__GI___libc_read>            ! 0x00007f08588c1c69
	cmp    $0x8,%rax                                       ! 0x00007f08588c1c6e
	setne  %al                                             ! 0x00007f08588c1c72
	add    $0x8,%rsp                                       ! 0x00007f08588c1c75
	movzbl %al,%eax                                        ! 0x00007f08588c1c79
	neg    %eax                                            ! 0x00007f08588c1c7c
	retq                                                   ! 0x00007f08588c1c7e
	nop                                                    ! 0x00007f08588c1c7f
	sub    $0x18,%rsp                                      ! 0x00007f08588c1c80
	mov    $0x8,%edx                                       ! 0x00007f08588c1c84
	mov    %rsi,0x8(%rsp)                                  ! 0x00007f08588c1c89
	lea    0x8(%rsp),%rsi                                  ! 0x00007f08588c1c8e
	callq  0x7f08588b0250  # <__GI___libc_write>           ! 0x00007f08588c1c93
	cmp    $0x8,%rax                                       ! 0x00007f08588c1c98
	setne  %al                                             ! 0x00007f08588c1c9c
	add    $0x18,%rsp                                      ! 0x00007f08588c1c9f
	movzbl %al,%eax                                        ! 0x00007f08588c1ca3
	neg    %eax                                            ! 0x00007f08588c1ca6
	retq                                                   ! 0x00007f08588c1ca8
	nopl   0x0(%rax)                                       ! 0x00007f08588c1ca9
	mov    %rcx,%r10                                       ! 0x00007f08588c1cb0
	mov    $0x12e,%eax                                     ! 0x00007f08588c1cb3
	syscall                                                ! 0x00007f08588c1cb8
	cmp    $0xfffffffffffff001,%rax                        ! 0x00007f08588c1cba
	jae    0x7f08588c1cc3  # <prlimit64+19>                ! 0x00007f08588c1cc0
	retq                                                   ! 0x00007f08588c1cc2
	mov    0x2c919e(%rip),%rcx        # 0x7f0858b8ae68     ! 0x00007f08588c1cc3
	neg    %eax                                            ! 0x00007f08588c1cca
	mov    %eax,%fs:(%rcx)                                 ! 0x00007f08588c1ccc
	or     $0xffffffffffffffff,%rax                        ! 0x00007f08588c1ccf
	retq                                                   ! 0x00007f08588c1cd3
	nopw   %cs:0x0(%rax,%rax,1)                            ! 0x00007f08588c1cd4
	xchg   %ax,%ax                                         ! 0x00007f08588c1cde
	mov    $0x87,%eax                                      ! 0x00007f08588c1ce0
	syscall                                                ! 0x00007f08588c1ce5
	cmp    $0xfffffffffffff001,%rax                        ! 0x00007f08588c1ce7
	jae    0x7f08588c1cf0  # <personality+16>              ! 0x00007f08588c1ced
	retq                                                   ! 0x00007f08588c1cef
	mov    0x2c9171(%rip),%rcx        # 0x7f0858b8ae68     ! 0x00007f08588c1cf0
	neg    %eax                                            ! 0x00007f08588c1cf7
	mov    %eax,%fs:(%rcx)                                 ! 0x00007f08588c1cf9
	or     $0xffffffffffffffff,%rax                        ! 0x00007f08588c1cfc