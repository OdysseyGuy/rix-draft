#ifndef PG_ALLOC
#define PG_ALLOC

#include <stddef.h>
#include <stdint.h>

#define _in_
#define _out_

void *
kpage_first_alloc(_out_ size_t *size_return);

#endif /* !PG_ALLOC */