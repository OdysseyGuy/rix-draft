/* SPDX-License-Identifier: MIT */

#include <processor.h>

list_t          processor_list;
uint32_t        processor_count;

processor_set_t pset;

extern list_t   thread_list;
extern uint32_t thread_count;

processor_t     bsp; /* bootstrap processor */


void
processor_bootstrap(void)
{
    /* Intialize the bootstrap processor */
    processor_init(&bsp, 0, &pset);
}


/**
 * @details
 * Processor intialization routine. Called once per proccessor.
 * Assigns the processor to the processor set.
 * 
 * Sets the proccessor in the idle state.
 */
void
processor_init(
    processor_t        *processor,
    uint32_t            num,
    processor_set_t    *processor_set)
{
    processor->cpu_number = num;
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

