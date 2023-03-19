#ifndef STDLIB_H
#define STDLIB_H


#define ROUND_UP(addr, align)    \
    ( (( (uintptr_t)addr ) + (( (uintptr_t)align ) - 1)) & ~(( (uintptr_t)align ) - 1) )

#define ROUND_DOWN(addr, align)  \
    ( ( (uintptr_t)addr ) & ~( ( (uintptr_t)align ) - 1 ) )


#define ALIGN(addr, align) ROUND_UP(addr, align)
#define IS_ALIGNED(addr, align) ( !( ((uintptr_t)addr) & (((uintptr_t)align) - 1) ) )


#endif /* STDLIB_H */
