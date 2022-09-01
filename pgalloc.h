#ifndef PGALLOC_H
#define PGALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <compiler.h>


void *
kpage_alloc(_in_ size_t pages);


size_t
kpage_free(
    _in_ void      *ptr,
    _in_ size_t     pages);


void *
kpage_first_alloc(_out_ size_t *size_return);


#endif /* PGALLOC_H */