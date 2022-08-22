#pragma once

#include <compiler.h>
#include <stdint.h>
#include <stddef.h>

#define _in_
#define _out_


void *
memchr(
    _in_ void const *,
    _in_ int        ,
    _in_ size_t     ) __pure;

int
memcmp(
    _in_ void const *a,
    _in_ void const *b,
    _in_ size_t     count) __pure;

void *
memset(
    _in_ void       *dest,
    _in_ int        c,
    _in_ size_t     count);

void *
memcpy(
    _in_ void       *to,
    _in_ void const *from,
    _in_ size_t     count);

void *
memmove(
    _in_ void       *,
    _in_ void const *,
    _in_ size_t     );


char *
strcat(
    _in_ char       *,
    _in_ char const );

char *
strchr(
    _in_ char const *,
    _in_ int        ) __pure;

int
strcmp(
    _in_ char const *,
    _in_ int        ) __pure;

uint8_t *
strcpy(
    _in_ char       *,
    _in_ char const *);