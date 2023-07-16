/* SPDX-License-Identifier: MIT */

#pragma once

#include <stdbool.h>
#include <thread.h>

typedef struct processor_set processor_set_t;

typedef enum processor_state {
    PROCESSOR_STATE_NA = 0,      /* Not Available */
    PROCESSOR_STATE_SHUTDOWN,    /* Going offline */
    PROCESSOR_STATE_STARTING,    /* Being started */
    PROCESSOR_STATE_IDLE,        /* Idle */
    PROCESSOR_STATE_DISPATCHING, /* Dispatching (Idle -> Running) */
    PROCESSOR_STATE_RUNNING,     /* Running */
} processor_state_t;

struct processor_set {
    list_node_t pset_list_node; /* List of processor sets. */
    list_t   processor_list;  /* List of processors belonging to this processor
                                 set. */
    uint32_t processor_count; /* Count of processor in this set. */

    list_t   threads;
    uint32_t thread_count; /* Total threads assigned to this processor set. */

    list_t   tasks;        /* Tasks assigned. */
    uint32_t task_count;   /* Count of task assigned. */
};

typedef struct processor {
    list_node_t pset_node;
    list_node_t plist_node;

    bool is_smt;              /* Simultaneous multithreading. */

    processor_state_t state;  /* Current processor state. */
    thread_t *current_thread; /* Thread currently running on this processor. */
    thread_t  idle_thread;

    uint32_t         cpu_number;
    processor_set_t *pset; /* Processor set this processor belongs to. */
} processor_t;

extern list_t processor_list;

/**
 * Initialize the processor set system.
 */
void pset_init(processor_set_t *pset);

void pset_add_processor(processor_t *processor, processor_set_t *pset);
void pset_remove_processor(processor_t *processor, processor_set_t *pset);

void pset_add_thread(processor_set_t *pset, thread_t *thread);
void pset_remove_thread(processor_set_t *pset, thread_t *thread);

/**
 * Initialize the processor set system and the bootstrap
 * processor.
 */
void processor_bootstrap(void);

/**
 * Initialize a processor object and assign it to a processor-set.
 *
 * @param processor Processor object to initialize.
 *
 * @param num Processor number.
 *
 * @param processor_set Processor-set that includes this processor.
 */
void processor_init(processor_t *processor, uint32_t num,
                    processor_set_t *processor_set);
