//
// Created by kingdo on 2020/10/22.
//

#ifndef TPFAAS_PRINT_H
#define TPFAAS_PRINT_H

#include <stdio.h>
#include <stdbool.h>

static inline void never_reach(const char *msg) {
    printf("%s\n", msg);
    printf("123\n");
    int m = 0;
    while (1 / m)
        m++;
}

#endif //TPFAAS_PRINT_H
