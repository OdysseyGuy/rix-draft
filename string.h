#pragma once

#include <compiler.h>
#include <stdint.h>
#include <stddef.h>

void * memchr(_in_ void const *ptr, _in_ int value, _in_ size_t count) __pure;
int memcmp(_in_ void const *a, _in_ void const *b, _in_ size_t count) __pure;
void * memset(_in_ void *dest, _in_ int c, _in_ size_t count);
void * memcpy(_in_ void *dest, _in_ void const *src, _in_ size_t count);
void * memmove(_in_ void *dest, _in_ void const *src, _in_ size_t count);