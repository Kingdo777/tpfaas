//
// Created by kingdo on 2020/10/22.
//

#ifndef TPFAAS_PRINT_H
#define TPFAAS_PRINT_H

#include <stdio.h>
#include <stdbool.h>

#define PRINT_LONG(n) print_num(n,true)
#define PRINT_INT(n) print_num((long)n,true)
#define PRINT_LONG_INLINE(n) print_num(n,false)
#define PRINT_INT_INLINE(n) print_num((long)n,false)

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

#endif //TPFAAS_PRINT_H
