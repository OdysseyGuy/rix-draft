#pragma once

#include <compiler.h>
#include <stdint.h>
#include <stddef.h>

void *memchr(void const *ptr, int value, size_t count) __pure;
int memcmp(void const *a, void const *b, size_t count) __pure;
void *memset(void *dest, int c, size_t count);
void *memcpy(void *dest, void const *src, size_t count);
void *memmove(void *dest, void const *src, size_t count);
