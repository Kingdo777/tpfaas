//
// Created by kingdo on 10/19/20.
//

#include "tls.h"

#include <asm/prctl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

static void call_arch_prctl(int code, unsigned long *addr);

void get_tls(void *tls_addr) {
    call_arch_prctl(ARCH_GET_FS, tls_addr);
}

void set_tls(void *tls_addr) {
    call_arch_prctl(ARCH_SET_FS, tls_addr);
}

void get_tls_gs(void *tls_addr) {
    call_arch_prctl(ARCH_GET_GS, tls_addr);
}

void set_tls_gs(void *tls_addr) {
    call_arch_prctl(ARCH_SET_GS, tls_addr);
}

static void call_arch_prctl(int code, unsigned long *addr) {
    if (arch_prctl(code, addr) == -1) {
        char *op = "";
        switch (code) {
            case ARCH_GET_FS:
                op = "get_tls";
                break;
            case ARCH_SET_FS:
                op = "set_tls";
                break;
            case ARCH_GET_GS:
                op = "get_tls_gs";
                break;
            case ARCH_SET_GS:
                op = "set_tls_gs";
                break;
            default:
                break;
        }
        switch (errno) {
            case EFAULT:
                printf("%s run wrong:addr points to an unmapped address or is outside the process address space.\n",
                       op);
                break;
            case EINVAL:
                printf("%s run wrong:code is not a valid subcommand.\n", op);
                break;
            case EPERM:
                printf("%s run wrongaddr is outside the process address space.\n", op);
                break;
            case ENODEV:
                printf("%s run wrongARCH_SET_CPUID was requested, but the underlying hardware does not support CPUID faulting.\n",
                       op);
                break;
            default:
                printf("%s run wrong:Unknown error(errno=%d)\n", op, errno);
        }
        exit(1);
    }
}