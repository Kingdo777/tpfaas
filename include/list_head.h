//
// Created by kingdo on 10/19/20.
//

#ifndef TPFAAS_LIST_HEAD_H
#define TPFAAS_LIST_HEAD_H

#include <stddef.h>

//双向链表，后添加的节点是表头
typedef struct list_head {
    struct list_head *next, *prev;
} list_head;

//  一个典型的list_head结构:
//
//  +------------------------------------------------------------------------------------------------------+
//  |                     +-------+        +-------+        +-------+        +-------+        +-------+    |
//  |                     |   v1  |        |   v1  |        |   v1  |        |   v1  |        |   v1  |    |
//  |                     |   v2  |        |   v2  |        |   v2  |        |   v2  |        |   v2  |    |
//  |    +-------+        +-------+        +-------+        +-------+        +-------+        +-------+    |
//  +--->|  next |------->|  next |------->|  next |------->|  next |------->|  next |------->|  next |----+
//       +-------+        +-------+        +-------+        +-------+        +-------+        +-------+
//  +----| prev  |<-------| prev  |<-------| prev  |<-------| prev  |<-------| prev  |<-------| prev  |<---+
//  |    +-------+        +-------+        +-------+        +-------+        +-------+        +-------+    |
//  |                                                                                                      |
//  +------------------------------------------------------------------------------------------------------+
//
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


typedef struct {
    struct list_head *head, *tail;
} list_head_chain;

//初始化并返回一个list_head结构
#define LIST_HEAD_INIT(name) { &(name), &(name) }
//创建一个表头
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name);

//配置未初始化的task_head
//配置未初始化的task_head
static inline void INIT_LIST_HEAD(struct list_head *list) {
    list->next = list;
    list->prev = list;
}

///******************** 添加节点 ****************************///

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
void list_add_(struct list_head *new, struct list_head *prev, struct list_head *next);

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
void list_add_tail(struct list_head *new, struct list_head *head);

/**
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
void list_add(struct list_head *new, struct list_head *head);

///******************** 链表遍历 ****************************///

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)


/**
 * 遍历链表,下面都是遍历所需要的子宏
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry(pos, head, member)                \
    for (pos = list_first_entry(head, typeof(*pos), member);    \
         &pos->member != (head);                    \
         pos = list_next_entry(pos, member))

#define list_for_each_entry_prev(pos, head, member)                \
    for (pos = list_first_entry_prev(head, typeof(*pos), member);    \
         &pos->member != (head);                    \
         pos = list_next_entry_prev(pos, member))

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 */

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */

#define take_an_entry_from_head(pos, head, member) do{ \
        pos=NULL;\
        if(!list_empty(head)){                             \
            pos = list_first_entry(head, typeof(*pos), member); \
        }                                                       \
    }while(0);
#define take_an_entry_from_tail(pos, head, member) do{ \
    pos=NULL;\
    if(!list_empty(head)){                             \
       pos = list_first_entry_prev(head, typeof(*pos), member); \
    }                                                       \
    }while(0);
#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#define list_first_entry_prev(ptr, type, member) \
    list_entry((ptr)->prev, type, member)

/**
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_next_entry(pos, member) \
    list_entry((pos)->member.next, typeof(*(pos)), member)

#define list_next_entry_prev(pos, member) \
    list_entry((pos)->member.prev, typeof(*(pos)), member)

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({          \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) );})

///******************** 删除节点 ****************************///

/**
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
void list_del__(struct list_head *prev, struct list_head *next);

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
void list_del_entry__(struct list_head *entry);

void list_del(struct list_head *entry);

///******************** 是否为空 ****************************///
int list_empty(const struct list_head *head);


///******************** 迁移节点 ****************************///

void list_move(struct list_head *list, struct list_head *head);

void list_move_tail(struct list_head *list, struct list_head *head);

//把链表A中的n个节点移动到B中，且从B的尾部添加
void move_listA_n_node_2_listB_tail(struct list_head *listA, struct list_head *listB, int n);

//把链表A中的n个节点移动到B中，且从B的头部添加
void move_listA_n_node_2_listB(struct list_head *listA, struct list_head *listB, int n);

void splice_tow_list_chain(list_head_chain *l1, list_head_chain *l2);

int try_take_a_N_size_chain_from_list(list_head_chain *l_chain, list_head *head, int N);

void append_a_list_chain_2_list_head(list_head_chain *l_chain, list_head *head);

//#define  change_l_chain_head(l_chain, new_member, type, member) \
//    l_chain.head=(container_of(&l_chain.head,type, member)).new_member;\
//    l_chain.tail=(container_of(&l_chain.head,type, member)).new_member;

#endif //TPFAAS_LIST_HEAD_H
