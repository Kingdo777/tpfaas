//
// Created by kingdo on 2020/10/23.
//

#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include "resource.h"
#include "tool/list_head.h"

LIST_HEAD(res_list_head);

void init_R(R *r,resource res) {
    INIT_R_LIST_HEAD(r)
    r->res = res;
    r->task_idle_count = 0;
    r->task_busy_count = 0;
    pthread_mutex_init(&r->task_busy_lock, NULL);
    pthread_mutex_init(&r->task_idle_lock, NULL);
}

R *create_R(resource res) {
    R *r = (R *) malloc(sizeof(R));
    init_R(r,res);
    list_add(&r->res_list, &res_list_head);
    return r;
}

R *get_res(resource res) {
    R *r;
    list_for_each_entry(r, &res_list_head, res_list) {
        if (RESOURCE_EQUAL(r->res, res))
            return r;
    }
    r = create_R(res);
    return r;
}

