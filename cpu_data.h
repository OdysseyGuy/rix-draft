/* SPDX-License-Identifier: MIT */

#ifndef PERCPU_H
#define PERCPU_H

#include <stdint.h>
#include <processor.h>
#include <compiler.h>
#include <x86.h>

/**
 * @brief
 * CPU-specific data. Each CPU in SMP holds this struct.
 * Use the get_current_cpu_data() function to get this struct
 * for the current CPU.
 */
typedef struct cpu_data {
    uint32_t        cpu_number;
    thread_t        cpu_current_thread;

    processor_t    *cpu_processor;

    uint8_t         cpu_lapic_id;
    uint8_t         cpu_lapic_version;
} cpu_data_t;

extern cpu_data_t *cpu_data_ptr[];

/**
 * @brief
 * Initialize the cpu data for boot processor.
 */
void boot_cpu_init(void);

/**
 * @brief
 * Called once by the boot processor after VM and heap is online to
 * initialize the cpu data for secondary processors.
 */
void secondary_cpus_init(void);

/**
 * @brief
 * Get the cpu data object.
 * 
 * @param cpu
 * CPU number.
 * 
 * @return
 * CPU data for the specified CPU number. 
 */
static inline cpu_data_t * get_cpu_data(_in_ uint32_t cpu)
{
    return cpu_data_ptr[cpu];
}

static inline cpu_data_t * get_current_cpu_data(void)
{
    return (cpu_data_t *)x86_get_gs_offset(__offsetof(cpu_data_t, cpu_number));
}

static inline thread_t * get_current_thread()
{
    return (thread_t *)x86_get_gs_offset(__offsetof(cpu_data_t, cpu_current_thread));
}

static inline uint32_t get_current_cpu_number(void)
{
    return (uint32_t)x86_get_gs_offset(__offsetof(cpu_data_t, cpu_number));
}


#endif /* PERCPU_H */
