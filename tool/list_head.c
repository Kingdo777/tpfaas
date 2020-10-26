//
// Created by kingdo on 2020/10/23.
//

#include "list_head.h"


#include <stddef.h>

///******************** 添加节点 ****************************///

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
void list_add_(struct list_head *new, struct list_head *prev, struct list_head *next) {
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
void list_add_tail(struct list_head *new, struct list_head *head) {
    list_add_(new, head->prev, head);
}

/**
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
void list_add(struct list_head *new, struct list_head *head) {
    list_add_(new, head, head->next);
}

///******************** 删除节点 ****************************///

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
void list_del__(struct list_head *prev, struct list_head *next) {
    next->prev = prev;
    prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
void list_del_entry__(struct list_head *entry) {
//    if (!__list_del_entry_valid(entry))
//        return;

    list_del__(entry->prev, entry->next);
}

void list_del(struct list_head *entry) {
    list_del_entry__(entry);
    entry->next = NULL;
    entry->prev = NULL;
}

///******************** 是否为空 ****************************///
int list_empty(const struct list_head *head) {
    return head->next == head;
}


///******************** 迁移节点 ****************************///

void list_move(struct list_head *list, struct list_head *head) {
    list_del__(list->prev, list->next);
    list_add(list, head);
}

void list_move_tail(struct list_head *list, struct list_head *head) {
    list_del__(list->prev, list->next);
    list_add_tail(list, head);
}

//下面两个函数,不敢用啊,因为会嵌使用锁;
//把链表A中的n个节点(向后找，队列式取出)移动到B中，且从B的尾部添加
void move_listA_n_node_2_listB_tail(struct list_head *listA, struct list_head *listB, int n) {
    for (int i = 0; i < n; i++) {
        if (list_empty(listA))
            break;
        list_move_tail(listA->prev, listB);
    }
}

//把链表A中的n个节点(向后找，队列式取出)移动到B中，且从B的头部添加
void move_listA_n_node_2_listB(struct list_head *listA, struct list_head *listB, int n) {
    for (int i = 0; i < n; i++) {
        if (list_empty(listA))
            break;
        list_move(listA->prev, listB);
    }
}

//  一个list_head_chain
//  +------------------------------------------------------------------------------------------------------+
//  |                                 +---------------------------------------------------+                |
//  |                     +-------+   |    +-------+        +-------+        +-------+    |   +-------+    |
//  |                     |   v1  |   |    |   v1  |        |   v1  |        |   v1  |    |   |   v1  |    |
//  |                     |   v2  |   |    |   v2  |        |   v2  |        |   v2  |    |   |   v2  |    |
//  |    +-------+        +-------+   |    +-------+        +-------+        +-------+    |   +-------+    |
//  +--->|  next |------->|  next |---+    |  next |------->|  next |------->|  next |    +-->|  next |----+
//       +-------+        +-------+        +-------+        +-------+        +-------+        +-------+
//  +----| prev  |<-------| prev  |<--+    | prev  |<-------| prev  |<-------| prev  |    +---| prev  |<---+
//  |    +-------+        +-------+   |    +-------+        +-------+        +-------+    |   +-------+    |
//  |                                 |       |                                  |        |                |
//  |                                 +-------+----------------------------------+--------+                |
//  +-----------------------------------------+----------------------------------+-------------------------+
//                                            |                                  |
//                                            |                                  |
//                                            |             +-------+            |
//                                            +--<-----<----|  head |            |
//                                                          +-------+            |
//                                                          | tail  |---->--->---+
//                                                          +-------+

// 把l2追接到l1上
void splice_tow_list_chain(list_head_chain *l1, list_head_chain *l2) {
    if (l1->head != l2->head && l1->tail != l2->tail) {
        l1->tail->next = l2->head;
        l2->head->prev = l1->tail;
        l1->tail = l2->tail;
    }
}

// 从task_list中尝试拉取N个结点,返回值为成功拉取的个数
int try_take_a_N_size_chain_from_list(list_head_chain *l_chain, list_head *head, int N) {
    int i = 0;
    list_head *pos;
    list_for_each(pos, head) {
        if (i == 0) {
            l_chain->head = pos;
            l_chain->tail = pos;
        } else if (i < N) {
            l_chain->tail = pos;
        } else {
            break;
        }
        i++;
    }
    return i;
}

void append_a_list_chain_2_list_head(list_head_chain *l_chain, list_head *head) {
    head->next->prev = l_chain->tail;
    l_chain->tail->next = head->next;
    head->next = l_chain->head;
    l_chain->head->prev = head;
}
