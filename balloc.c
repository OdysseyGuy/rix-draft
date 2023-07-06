/* SPDX-License-Identifier: MIT */

#include <stdlib.h>
#include <balloc.h>

/* kernel.lds.S */
extern long __end;

uintptr_t boot_alloc_start = (uintptr_t)&__end;
uintptr_t boot_alloc_end = (uintptr_t)&__end;

void * balloc(size_t len)
{
    /* start allocating after boot_alloc_end */
    uintptr_t ptr = ALIGN(boot_alloc_end, 8);
    boot_alloc_end = (ptr + ALIGN(len, 8));
    return (void *)ptr;
}
