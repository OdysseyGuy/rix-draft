/* SPDX-License-Identifier: MIT */

#include <x86.h>
#include <apic.h>
#include <platform.h>

#define NUM_ISR 256

/**
 * An interrupt service routine.
 */
typedef void (*isr_ptr_t)(void *arg);

/**
 * Represents a single entry in the interrupt handler table.
 */
typedef struct int_table_entry {
    isr_ptr_t   callback;       /* Pointer to the ISR. */
    void       *arg;            /* Arguments to the ISR. */
    uint32_t    allocated : 1,  /* Whether or not this ISR was
                                 * allocated.
                                 */
    edge : 1;                   /* Edge(1)/Level(0) triggered. */
} int_table_entry_t;

/**
 * Interrupt handler table.
 */
static int_table_entry_t int_table[NUM_ISR];

void platform_init_interrupts(void)
{
    sizeof(int_table_entry_t);
}

void platform_int_handler(x86_interrupt_frame_t* frame)
{
    uint32_t vector = frame->vector;
    int_table_entry_t* handler = &int_table[vector];

    /* edge triggered interrupt */
    if (handler->edge) {
    }

    /* invoke the registered callback */
    if (handler->callback) {
        handler->callback(handler->arg);
    }

    /* level triggered interrupt */
    if (!handler->edge) {
    }
}

void register_isr(uint32_t vector, isr_ptr_t callback, bool edge)
{
    int_table[vector].callback = callback;
    int_table[vector].allocated = true;
    int_table[vector].edge = edge;
}
