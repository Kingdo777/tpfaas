//
// Created by kingdo on 2020/10/23.
//

#ifndef TPFAAS_QUEUE_H
#define TPFAAS_QUEUE_H

#include <stdbool.h>
#include "tool/list_head.h"

/**
 *    F的Queue队列结构
 *
 *    +-----+      +------------+      +------------+      +------------+      +------------+
 *    +  F  +----->+F_queue_list+----->+F_queue_list+----->+F_queue_list+----->+F_queue_list+-
 *    +-----+      +------------+      +------------+      +------------+      +------------+
 *                        |                    |                  |                   |
 *                     +-----+              +-----+            +-----+             +-----+
 *                     +  I  +              +  I  +            +  I  +             +  I  +
 *                     +-----+              +-----+            +-----+             +-----+
 *                        |                    |                  |                   |
 *                     +-----+              +-----+            +-----+             +-----+
 *                     +  I  +              +  I  +            +  I  +             +  I  +
 *                     +-----+              +-----+            +-----+             +-----+
 *                        |                    |                  |                   |
 *                     +-----+              +-----+            +-----+             +-----+
 *                     +  I  +              +  I  +            +  I  +             +  I  +
 *                     +-----+              +-----+            +-----+             +-----+
 *                        |                    |                  |                   |
 *                     +-----+              +-----+            +-----+             +-----+
 *                     +  I  +              +  I  +            +  I  +             +  I  +
 *                     +-----+              +-----+            +-----+             +-----+
 *                        |                    |                  |                   |
 *                     +-----+              +-----+            +-----+             +-----+
 *                     +  I  +              +  I  +            +  I  +             +  I  +
 *                     +-----+              +-----+            +-----+             +-----+
 */

typedef struct {
    //queue_list上当前I的数目
    int queue_list_size;
    //queue_list上最大可容纳I的数目
    int queue_list_max_cap;
    //表头，链接队列中的所有I
    list_head instance_list_head;
} queue_list;

typedef struct {
    //是否为等待队列
    bool wait_queue;
    queue_list q_list;
    //表头在F，链接所有的queue_head
    list_head F_queue_list_list;

} F_queue_list;


#endif //TPFAAS_QUEUE_H
