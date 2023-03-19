/* SPDX-License-Identifier: MIT */

#include <stddef.h>
#include <pmm.h>
#include <pgalloc.h>

void * kpage_alloc(size_t pages)
{
    void *result = pmm_alloc_kpages(pages, NULL);
    return result;
}

size_t kpage_free(void *ptr, size_t pages)
{
    return pmm_free_kpages(ptr, pages);
}

void * kpage_first_alloc(size_t *size_return)
{
    *size_return = PAGE_SIZE;
    return kpage_alloc(1);
}