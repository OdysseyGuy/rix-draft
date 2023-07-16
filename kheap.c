/* SPDX-License-Identifier: MIT */

#include <kheap.h>
#include <list.h>
#include <stdint.h>
#include <types.h>
#include <pgalloc.h>
#include <stdlib.h>

typedef struct free_heap_chunk {
    list_node_t node;
    size_t      len;
} free_heap_chunk_t;

typedef struct heap {
    void  *base;
    size_t len;
    size_t remaining;
    list_t free_list;
} heap_t;

typedef struct alloc_struct {
    void  *ptr;
    size_t size;
} alloc_struct_t;

static heap_t kheap;

void *kheap_malloc(size_t size)
{
}

void *kheap_calloc(size_t count, size_t size)
{
}

void *kheap_realloc(void *ptr, size_t size)
{
}

void kheap_free(void *ptr)
{
}

static free_heap_chunk_t *heap_insert_free_chunk(free_heap_chunk_t *chunk)
{
    vaddr_t chunk_end = (vaddr_t)chunk + chunk->len;

    kheap.remaining += chunk->len;

    free_heap_chunk_t *next_chunk;
    list_for_each_entry (next_chunk, &kheap.free_list, node) {
        if (chunk < next_chunk) {
            list_add_tail(&next_chunk->node, &chunk->node);
            goto try_merge;
        }
    }

    list_add(&kheap.free_list, &chunk->node);

try_merge:;
}

static free_heap_chunk_t *heap_create_free_chunk(void *ptr, size_t len)
{
    free_heap_chunk_t *chunk = (free_heap_chunk_t *)ptr;
    chunk->len = len;
    return chunk;
}

void kheap_init(void)
{
    size_t len;
    void  *ptr = kpage_first_alloc(&len);

    /* initialize the free list */
    list_init(&kheap.free_list);

    /* align the buffer to a ptr */
    if (((uintptr_t)ptr % sizeof(uintptr_t)) > 0) {
        uintptr_t aligned_ptr = ROUND_UP((uintptr_t)ptr, sizeof(uintptr_t));

        len -= aligned_ptr - (uintptr_t)ptr;
        ptr = (void *)aligned_ptr;
    }

    kheap.base = ptr;
    kheap.len = len;
    kheap.remaining = 0;

    if (len > 0) {
        heap_insert_free_chunk(heap_create_free_chunk(ptr, len));
    }
}
