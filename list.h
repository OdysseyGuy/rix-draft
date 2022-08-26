/* SPDX-License-Identifier: MIT */

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stddef.h>

#define _in_
#define _out_

#define container_of(ptr, type, member) \
    ((type *)((addr_t)(ptr) - offsetof(type, member)))


/**
 * @brief
 * Circular doubly linked-list.
 */
typedef struct list_node {
    struct list_node *prev;
    struct list_node *next;
} list_node_t, list_t, queue_node_t, queue_t;


#define LIST_INITIAL_VALUE(value)   { &(value), &(value) }
#define LIST_INITIAL_CLEARED_VALUE  { NULL, NULL }

#define list_entry(ptr, type, member) container_of(ptr, type, member)


static inline void
list_init(_in_ list_node_t *list)
{
    list->prev = list->next = list;
}

static inline void
list_clear(_in_ list_node_t *entry)
{
    entry->prev = entry->next = 0;
}

/**
 * @brief
 * Whether or not this list is empty.
 * Empty list means the next and previous points to the head itself.
 */
static inline bool
list_is_empty(_in_ list_node_t *list)
{
    return list->prev == list && list->next == list;
}

/**
 * @brief
 * Whether or not the head belongs to a list.
 */
static inline bool
list_is_in_list(_in_ list_node_t *head)
{
    if (head->next == 0 && head->prev == 0) {
        return false;
    }
    return true;
}


/**
 * @brief
 * Insert a new entry after the specified head.
 */
static inline void
list_add(_in_ list_node_t *head, _in_ list_node_t *new_entry)
{
    new_entry->next = head->next;
    new_entry->prev = head;

    head->next->prev = new_entry;
    head->next = new_entry;
}

/**
 * @brief
 * Insert a new entry before the specified head.
 */
static inline void
list_add_tail(_in_ list_node_t *head, _in_ list_node_t *new_entry)
{
    new_entry->prev = head->prev;
    new_entry->next = head;
    
    head->prev->next = new_entry;
    head->prev = new_entry;
}


/**
 * @brief
 * Replace old entry with new one.
 */
static inline void
list_replace(
    _in_ list_node_t *old_item,
    _in_ list_node_t *new_entry)
{
    new_entry->next = old_item->next;
    new_entry->prev = old_item->prev;

    old_item->prev->next = new_entry;
    old_item->next->prev = new_entry;
}

static inline void
list_delete(_in_ list_node_t *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
    
    entry->prev = entry->next = 0;
}


/**
 * @brief
 * Removes the head from the list and returns it.
 */
static inline list_node_t *
list_remove_head(_in_ list_node_t *head)
{
    if (head->next != head) {
        list_node_t* item = head->next;
        list_delete(item);
        return item;
    } else {
        return NULL;
    }
}

#define list_remove_head_type(head, type, element)      \
    ({                                                  \
        list_node_t* node = list_remove_head(head);     \
        type* __t;                                      \
        if (node)                                       \
            __t = container_of(node, type, element);    \
        else                                            \
            __t = (type*)0;                             \
        __t;                                            \
    })

/**
 * @brief
 * Removes the tail from the list and returns it.
 */
static inline list_node_t *
list_remove_tail(_in_ list_node_t *list)
{
    if (list->prev != list) {
        list_node_t* item = list->prev;
        list_delete(item);
        return item;
    } else {
        return NULL;
    }
}

#define list_remove_tail_type(list, type, element)      \
    ({                                                  \
        list_node_t* node = list_remove_tail(list);     \
        type* __t;                                      \
        if (node)                                       \
            __t = container_of(node, type, element);    \
        else                                            \
            __t = (type*)0;                             \
        __t;                                            \
    })

/**
 * @brief
 * Returns the next entry from the list.
 */
static inline list_node_t *
list_peek_head(_in_ list_node_t *list)
{
    if (list->next != list) {
        return list->next;
    } else {
        return NULL;
    }
}

#define list_peek_head_type(list, type, element)        \
    ({                                                  \
        list_node_t *node = list_peek_head(list);       \
        type* __t;                                      \
        if(node)                                        \
            __t = container_of(node, type, element);    \
        else                                            \
            __t = (type *)0;                            \
        __t;                                            \
    })

/**
 * @brief
 * Returns the previous entry from the list.
 */
static inline list_node_t *
list_peek_tail(_in_ list_node_t *list)
{
    if (list->prev != list) {
        return list->prev;
    } else {
        return NULL;
    }
}

#define list_peek_tail_type(list, type, element)        \
    ({                                                  \
        list_node_t *node = list_peek_tail(list);       \
        type* __t;                                      \
        if(node)                                        \
            __t = container_of(node, type, element);    \
        else                                            \
            __t = (type *)0;                            \
        __t;                                            \
    })


/**
 * @brief
 * Iterate over the list.
 */
#define list_for_each(pos, head)                                        \
    for (pos = head->next; pos != (head); pos = pos->next)

/**
 * @brief
 * Iterate over the list backwards.
 */
#define list_for_each_prev(pos, head)                                   \
    for (pos = head->prev; pos != (head); pos = pos->pev)

/**
 * @brief
 * Iterates of over every list, entry should be the container
 * structure type.
 */
#define list_for_each_entry(pos, head, member)                          \
    for(pos = container_of((head)->next, typeof(*pos), member);         \
        &pos->member != (head);                                         \
        pos = container_of((pos)->member.next, typeof(*pos), member))


/**
 * @brief
 * Returns the size of the list.
 */
static inline size_t
list_length(_in_ const list_node_t *list)
{
    size_t cnt = 0;
    const list_node_t* node = list;
    list_for_each(list, node) { cnt++; }
    return cnt;
}

/**
 * @brief
 * Split the contents of list after (but not including) pos.
 */
static inline void
list_split_after(
    _in_ list_node_t    *list,
    _in_ list_node_t    *pos,
    _out_ list_node_t   *split_to)
{
    if (pos->next == list) {
        list_init(split_to);
        return;
    }

    split_to->prev = list->prev;
    split_to->prev->next = split_to;
    split_to->next = pos->next;
    split_to->next->prev = split_to;
    
    pos->next = list;
    list->prev = pos;
}

static inline void
list_splice_after(_in_ list_node_t* splice_from, _in_ list_node_t* pos)
{
    if (list_is_empty(splice_from)) {
        list_init(splice_from);
    }
}

#endif /* LIST_H */
