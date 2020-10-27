//
// Created by kingdo on 2020/10/22.
//

#ifndef TPFAAS_PRINT_H
#define TPFAAS_PRINT_H

#include <stdio.h>
#include <stdbool.h>
#include "tool/tls.h"

extern unsigned long printf_fs;
extern unsigned long base;

#define PRINT_LONG(n) print_num(n,true)
#define PRINT_INT(n) print_num((long)n,true)
#define PRINT_LONG_INLINE(n) print_num(n,false)
#define PRINT_INT_INLINE(n) print_num((long)n,false)

static inline void never_reach(const char *msg) {
    puts(msg);
    int m = 0;
    while (1 / m)
        m++;
}

static inline void print_num(long num, bool new_line) {
    char s[50];
    int i = 49;
    s[i--] = '\0';
    if (new_line)
        s[i--] = '\n';
    char sign = num < 0 ? (num = 0 - num, '-') : 0;
    if (!num)
        s[i] = '0';
    else
        for (long j = num % 10; num != 0; num /= 10, j = num % 10)
            s[i--] = '0' + j;
    if (sign)
        s[i--] = sign;
    puts(&s[++i]);
}


#define GET_PRINTF_FS() get_tls(&printf_fs);
#define RESTORE_PRINTF_FS() set_tls((void *) printf_fs);
#define SAVE_FS() get_tls(&base);
#define RESTORE_FS() set_tls((void *) base);
#define PRINTF(...) do{SAVE_FS() RESTORE_PRINTF_FS() printf(__VA_ARGS__); RESTORE_FS()}while(0);

#endif //TPFAAS_PRINT_H
