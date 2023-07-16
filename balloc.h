/* SPDX-License-Identifier: MIT */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <compiler.h>

extern uintptr_t boot_alloc_start;
extern uintptr_t boot_alloc_end;

/**
 * Simple boot allocator. Chews up space after __end symbol.
 */
void *balloc(size_t len);
