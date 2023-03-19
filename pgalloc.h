/* SPDX-License-Identifier: MIT */

#ifndef PGALLOC_H
#define PGALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <compiler.h>


void * kpage_alloc(size_t pages);
size_t kpage_free(void *ptr, size_t pages);
void * kpage_first_alloc(size_t *size);


#endif /* PGALLOC_H */
