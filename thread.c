/* SPDX-License-Identifier: MIT */

#include <thread.h>
#include <spinlock.h>
#include <cpu_data.h>


list_t          thread_list;
uint32_t        thread_count;
spin_lock_t     thread_lock = 0;

static queue_t  thread_queue;
static uint32_t thread_queue_bitmap;

void
thread_init_early(void)
{
    list_init(&thread_list);

    boot_cpu_init();

    thread_t *t = &get_cpu_data(0)->cpu_processor->idle_thread;
    create_bootstrap_thread(t);
}

static void
thread_queue_enqueue(thread_t *t)
{
}

void
thread_join(thread_t *t, uint64_t timeout)
{
}

void
create_bootstrap_thread(thread_t *t)
{
    uint32_t cpu_num = get_current_cpu_number();

    list_add(&thread_list, &t->thread_list);
}