/* SPDX-License-Identifier: MIT */

#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>

void *
kheap_malloc(_in_ size_t size);

void *
kheap_calloc(
    _in_ size_t count,
    _in_ size_t size);

void *
kheap_realloc(
    _in_ void   *ptr,
    _in_ size_t size);

void
kheap_free(_in_ void *ptr);

void
kheap_init(void);


#endif /* HEAP_H */