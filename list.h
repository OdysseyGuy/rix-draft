/* SPDX-License-Identifier: MIT */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <compiler.h>

#define container_of(ptr, type, member) \
    ((type *)((addr_t)(ptr) - offsetof(type, member)))

/**
 * Circular doubly linked-list.
 */
typedef struct list_node {
    struct list_node *prev;
    struct list_node *next;
} list_node_t, list_t, queue_node_t, queue_t;

#define LIST_INITIAL_VALUE(value)                                              \
    {                                                                          \
        &(value), &(value)                                                     \
    }
#define LIST_INITIAL_CLEARED_VALUE                                             \
    {                                                                          \
        NULL, NULL                                                             \
    }

#define list_entry(ptr, type, member) container_of(ptr, type, member)

/**
 * Initializes the list the setting the prev and next pointer to itself.
 */
static inline void list_init(list_node_t *list)
{
    list->prev = list->next = list;
}

static inline void list_clear(list_node_t *entry)
{
    entry->prev = entry->next = 0;
}

/**
 * Whether or not this list is empty.
 * Empty list means the next and previous points to the head itself.
 */
static inline bool list_is_empty(list_node_t *list)
{
    return list->prev == list && list->next == list;
}

/**
 * Whether or not the head belongs to a list.
 */
static inline bool list_is_list(list_node_t *head)
{
    if (head->next == 0 && head->prev == 0) {
        return false;
    }
    return true;
}

/**
 * Insert a new entry after the specified head.
 */
static inline void list_add(list_node_t *head, list_node_t *new_entry)
{
    new_entry->next = head->next;
    new_entry->prev = head;

    head->next->prev = new_entry;
    head->next = new_entry;
}

/**
 * Insert a new entry before the specified head.
 */
static inline void list_add_tail(list_node_t *head, list_node_t *new_entry)
{
    new_entry->prev = head->prev;
    new_entry->next = head;

    head->prev->next = new_entry;
    head->prev = new_entry;
}

/**
 * Replace old entry with new one.
 */
static inline void list_replace(list_node_t *old_entry, list_node_t *new_entry)
{
    new_entry->next = old_entry->next;
    new_entry->prev = old_entry->prev;

    old_entry->prev->next = new_entry;
    old_entry->next->prev = new_entry;
}

/**
 * Delete the specified entry from a list.
 */
static inline void list_delete(list_node_t *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;

    entry->prev = entry->next = 0;
}

/**
 * Removes the head from the list and returns it.
 */
static inline list_node_t *list_remove_head(list_node_t *head)
{
    if (head->next != head) {
        list_node_t *item = head->next;
        list_delete(item);
        return item;
    } else {
        return NULL;
    }
}

#define list_remove_head_type(head, type, element)                             \
    ({                                                                         \
        list_node_t *node = list_remove_head(head);                            \
        type *__t;                                                             \
        if (node)                                                              \
            __t = container_of(node, type, element);                           \
        else                                                                   \
            __t = (type *)0;                                                   \
        __t;                                                                   \
    })

/**
 * Removes the tail from the list and returns it.
 */
static inline list_node_t *list_remove_tail(list_node_t *list)
{
    if (list->prev != list) {
        list_node_t *item = list->prev;
        list_delete(item);
        return item;
    } else {
        return NULL;
    }
}

#define list_remove_tail_type(list, type, element)                             \
    ({                                                                         \
        list_node_t *node = list_remove_tail(list);                            \
        type *__t;                                                             \
        if (node)                                                              \
            __t = container_of(node, type, element);                           \
        else                                                                   \
            __t = (type *)0;                                                   \
        __t;                                                                   \
    })

/**
 * Returns the next entry from the list.
 */
static inline list_node_t *list_peek_head(list_node_t *list)
{
    if (list->next != list) {
        return list->next;
    } else {
        return NULL;
    }
}

#define list_peek_head_type(list, type, element)                               \
    ({                                                                         \
        list_node_t *node = list_peek_head(list);                              \
        type *__t;                                                             \
        if (node)                                                              \
            __t = container_of(node, type, element);                           \
        else                                                                   \
            __t = (type *)0;                                                   \
        __t;                                                                   \
    })

/**
 * Returns the previous entry from the list.
 */
static inline list_node_t *list_peek_tail(list_node_t *list)
{
    if (list->prev != list) {
        return list->prev;
    } else {
        return NULL;
    }
}

#define list_peek_tail_type(list, type, element)                               \
    ({                                                                         \
        list_node_t *node = list_peek_tail(list);                              \
        type *__t;                                                             \
        if (node)                                                              \
            __t = container_of(node, type, element);                           \
        else                                                                   \
            __t = (type *)0;                                                   \
        __t;                                                                   \
    })

/**
 * Iterate over the list.
 */
#define list_for_each(pos, head)                                               \
    for (pos = head->next; pos != (head); pos = pos->next)

/**
 * Iterate over the list backwards.
 */
#define list_for_each_prev(pos, head)                                          \
    for (pos = head->prev; pos != (head); pos = pos->pev)

/**
 * Iterates of over every list, entry should be the container structure type.
 */
#define list_for_each_entry(pos, head, member)                                 \
    for (pos = container_of((head)->next, __typeof__(*pos), member);           \
         &pos->member != (head);                                               \
         pos = container_of((pos)->member.next, __typeof__(*pos), member))

/**
 * Returns the size of the list.
 */
static inline size_t list_length(const list_node_t *list)
{
    size_t cnt = 0;
    const list_node_t *node = list;
    list_for_each(list, node)
    {
        cnt++;
    }
    return cnt;
}

/**
 * Split the contents of list after (but not including) pos. into split_to
 * (which should be empty).
 */
static inline void list_split_after(list_node_t *list, list_node_t *pos,
                                    list_node_t *split_to)
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

/**
 * Splice the contents of splice_from into the list immediately following pos.
 */
static inline void list_splice_after(list_node_t *splice_from, list_node_t *pos)
{
    if (list_is_empty(splice_from)) {
        return;
    }

    splice_from->next->prev = pos;
    splice_from->prev->next = pos->next;
    pos->next->prev = splice_from->prev;
    pos->next = splice_from->next;

    list_init(splice_from);
}

/**
 * Moves all the contents of old_list (which may or may not be empty) to
 * new_list (which should be empty).
 */
static inline void list_move(list_node_t *old_list, list_node_t *new_list)
{
    list_init(new_list);
    list_splice_after(old_list, new_list);
}
