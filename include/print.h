//
// Created by kingdo on 2020/10/22.
//

#ifndef TPFAAS_PRINT_H
#define TPFAAS_PRINT_H

#include <stdio.h>
#include <stdbool.h>

extern list_head func_list_head;

static inline void never_reach(const char *msg) {
    printf("%s\n", msg);
    int m = 0;
    while (1 / m)
        m++;
}

static inline void print_task_info() {
    R *r;
    F *f;
    int list_busy_count = 0, list_idle_count = 0;
    list_head *tmp_list;
    list_for_each_entry(f, &func_list_head, func_list_list) {
        r = f->r;
        list_for_each(tmp_list, &r->task_idle_head) {
            list_idle_count++;
        }
        list_for_each(tmp_list, &r->task_busy_head) {
            list_busy_count++;
        }
        printf("busy/idle list-busy/list-idle count:%d/%d %d/%d\n", r->task_busy_count, r->task_idle_count,
               list_busy_count, list_idle_count);
    }
}

#endif //TPFAAS_PRINT_H
