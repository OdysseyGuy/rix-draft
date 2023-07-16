/* SPDX-License-Identifier: MIT */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <compiler.h>

void  *kpage_alloc(size_t pages);
size_t kpage_free(void *ptr, size_t pages);
void  *kpage_first_alloc(size_t *size);
