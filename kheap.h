/* SPDX-License-Identifier: MIT */

#pragma once

#include <stddef.h>
#include <compiler.h>

void  kheap_init(void);
void *kheap_malloc(size_t size);
void *kheap_calloc(size_t count, size_t size);
void *kheap_realloc(void *ptr, size_t size);
void  kheap_free(void *ptr);
