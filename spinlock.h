#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdbool.h>
#include <stddef.h>

typedef size_t spin_lock_t;

static inline void
spin_lock_init(spin_lock_t *lock)
{
    *lock = 0;
}

static inline void
spin_lock_lock(spin_lock_t *lock)
{
    *lock = 1;
}

static inline bool
spin_lock_held(spin_lock_t *lock)
{
    return (!(*lock));
}

static inline bool
spin_lock_unlock(spin_lock_t *lock)
{
    *lock = 0;
}


#endif /* SPINLOCK_H */