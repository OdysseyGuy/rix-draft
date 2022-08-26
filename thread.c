#include <thread.h>
#include <spinlock.h>

static list_t thread_list;

spin_lock_t thread_lock = 0;

static queue_t thread_queue[];
static uint32_t thread_queue_bitmap;


static void
thread_queue_enqueue(thread_t *t)
{
}

void
thread_join(thread_t *t, uint64_t timeout)
{
}