/* SPDX-License-Identifier: MIT */

#pragma once

#include <stdint.h>
#include <list.h>
#include <compiler.h>

typedef struct processor_set processor_set_t;

typedef uint32_t thread_id_t;
typedef void (*thread_func_t)(void *arg);

typedef enum thread_state {
    THREAD_STATE_SUSPENDED,
    THREAD_STATE_WAITING,
    THREAD_STATE_RUNNING,
} thread_state_t;

typedef struct thread {
    thread_id_t id;          /* Thread ID */
    list_node_t thread_list; /* Thread list */
    list_node_t pset_threads;
    int         priority;    /* Thread priority*/

    thread_state_t state;    /* Current thread state */

    processor_set_t *pset;   /* Processor set this thread
                              * was assigned.
                              */

    void  *stack;            /* Thread stack */
    size_t stack_size;       /* Thread stack size */

    thread_func_t func;      /* Routine that this thread
                              * would execute.
                              */
    void         *arg;       /* Arguments for the routine */

    uint8_t name[32];        /* Thread name */
} thread_t;

void thread_init_early(void);
void thread_init(void);

/**
 * Put the thread to sleep.
 *
 * @param ms Time (in milliseconds) util the thread sleeps.
 */
void thread_sleep(uint64_t ms);

/**
 * Creates a new thread.
 *
 * @param name Name of the thread.
 *
 * @param func Threaded function to be executed.
 *
 * @param arg Arguments to pass to the threaded function.
 *
 * @param priority Thread execution priority.
 *
 * @param stack Pointer to the thread stack.
 *
 * @param stack_size Size of the thread stack.
 *
 * @return Pointer to the thread object or NULL on failure.
 */
thread_t *thread_create(uint8_t *name, thread_func_t func, void *arg,
                        int priority, void *stack, size_t stack_size);

/**
 * Sends the caller thread the waiting state and remains
 * waiting until the specified thread terminates.
 *
 * In case of timeout the control returns to the caller thread
 * whether or not the specified thread has completed execution.
 *
 * @param t Thread to wait for to continue execution.
 *
 * @param timeout Time to wait for the specified thread to finish.
 */
void thread_join(thread_t *t, uint64_t timeout);

/**
 * Terminates the current thread and returns the specified
 * return code.
 *
 * @param ret Return code.
 */
void thread_exit(int ret) __noreturn;

/**
 * Create initial thread responsible for booting up the system.
 */
void create_bootstrap_thread(thread_t *t);
