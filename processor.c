/* SPDX-License-Identifier: MIT */

#include <processor.h>

#define MAX_NCPUS   64

processor_t         processor_array[MAX_NCPUS];

processor_set_t     default_pset;

list_t              pset_list;
uint8_t             pset_count;

extern list_t       thread_list;
extern uint32_t     thread_count;

processor_t         bsp; /* bootstrap processor */


void
processor_bootstrap(void)
{
    list_init(&processor_list);

    list_init(&pset_list);
    list_add(&pset_list, &default_pset.pset_list_node);
    pset_count = 1;

    /* Initialize the bootstrap processor */
    processor_array[0] = bsp;
    processor_init(&bsp, 0, &default_pset);
}

void
pset_init(_in_ processor_set_t *pset)
{
    list_init(&pset->processor_list);
    pset->processor_count = 0;
    list_init(&pset->threads);
    pset->thread_count = 0;
    list_init(&pset->tasks);
    pset->task_count = 0;
}


/**
 * @details
 * Processor initialization routine. Called once per processor.
 * Assigns the processor to the processor set.
 * Sets the processor in the idle state.
 */
void
processor_init(
    processor_t        *processor,
    uint32_t            num,
    processor_set_t    *pset)
{
    processor->cpu_number = num;
    processor->pset = NULL;

    processor->idle_thread;
}

void
pset_add_processor(
    processor_t        *processor,
    processor_set_t    *pset)
{
    list_add(&pset->processor_list, &processor->pset_node);
    processor->pset = pset;
    pset->processor_count++;
}

void
pset_remove_processor(
    processor_t        *processor,
    processor_set_t    *pset)
{
    list_delete(&processor->pset_node);
    processor->pset = NULL;
    pset->processor_count--;
}

void
pset_add_thread(
    processor_set_t    *pset,
    thread_t           *thread)
{
    list_add(&pset->threads, &thread->pset_threads);
    thread->pset = pset;
    pset->thread_count++;
}

void
pset_remove_thread(
    processor_set_t    *pset,
    thread_t           *thread)
{
    list_delete(&thread->pset_threads);
    thread->pset = NULL;
    pset->thread_count--;
}

