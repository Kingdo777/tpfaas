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

//初始化并返回一个list_head结构
#define LIST_HEAD_INIT(name) { &(name), &(name) }
//创建一个表头
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)
//配置未初始化的task_head
static inline void INIT_LIST_HEAD(struct list_head *list) {
    list->next = list;
    list->prev = list;
}


//添加节点
static inline void list_add_(struct list_head *new,
                                struct list_head *prev,
                                struct list_head *next) {

    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head) {
    list_add_(new, head, head->next);
}


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
#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

/**
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_next_entry(pos, member) \
    list_entry((pos)->member.next, typeof(*(pos)), member)

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
#endif //TPFAAS_LIST_HEAD_H
