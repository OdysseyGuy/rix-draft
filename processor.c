/* SPDX-License-Identifier: MIT */

#include <processor.h>

<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
>>>>>>> Stashed changes
list_t          processor_list;
uint32_t        processor_count;

processor_set_t pset;
<<<<<<< Updated upstream
=======
=======
#define MAX_NCPUS   64

processor_t     processor_array[MAX_NCPUS];

processor_set_t default_pset;

list_t          pset_list;
uint8_t         pset_count;
>>>>>>> Stashed changes
>>>>>>> Stashed changes

extern list_t   thread_list;
extern uint32_t thread_count;

processor_t     bsp; /* bootstrap processor */


void
processor_bootstrap(void)
{
<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
>>>>>>> Stashed changes
    /* Intialize the bootstrap processor */
    processor_init(&bsp, 0, &pset);
}

<<<<<<< Updated upstream
=======
=======
    list_init(&processor_list);

    list_init(&pset_list);
    list_add(&pset_list, &default_pset.pset_list_node);
    pset_count = 1;

    /* Intialize the bootstrap processor */
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
>>>>>>> Stashed changes
>>>>>>> Stashed changes

/**
 * @details
 * Processor intialization routine. Called once per proccessor.
 * Assigns the processor to the processor set.
<<<<<<< Updated upstream
 * 
=======
<<<<<<< Updated upstream
 * 
=======
 *
>>>>>>> Stashed changes
>>>>>>> Stashed changes
 * Sets the proccessor in the idle state.
 */
void
processor_init(
    processor_t        *processor,
    uint32_t            num,
<<<<<<< Updated upstream
    processor_set_t    *processor_set)
{
    processor->cpu_number = num;
=======
<<<<<<< Updated upstream
    processor_set_t    *processor_set)
{
    processor->cpu_number = num;
=======
    processor_set_t    *pset)
{
    processor->cpu_number = num;
    processor->pset = NULL;

    processor->idle_thread;
>>>>>>> Stashed changes
>>>>>>> Stashed changes
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

