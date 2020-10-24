//
// Created by kingdo on 2020/10/23.
//

#include <stdbool.h>
#include <stdlib.h>
#include "resource.h"
#include "tool/list_head.h"

LIST_HEAD(res_list_head);

R *get_res(resource res) {
    R *r;
    list_for_each_entry(r, &res_list_head, res_list) {
        if (RESOURCE_EQUAL(r->res, res))
            return r;
    }
    r = (R *) malloc(sizeof(R));
    INIT_R_LIST_HEAD(r);
    list_add(&r->res_list, &res_list_head);
    return r;
}

