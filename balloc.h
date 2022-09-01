/* SPDX-License-Identifier: MIT */

#ifndef _BALLOC_H_
#define _BALLOC_H_

#include <stdint.h>
#include <stddef.h>
#include <compiler.h>


extern uintptr_t boot_alloc_start;
extern uintptr_t boot_alloc_end;

/**
 * @brief
 * Simple boot allocator. Chews up space after __end symbol.
 */
void *
balloc(_in_ size_t len);


#endif /* !_BALLOC_H_ */