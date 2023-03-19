/* SPDX-License-Identifier: MIT */

#include <cpu_data.h>

#define MAX_NCPUS 64

cpu_data_t  *cpu_data_ptr[MAX_NCPUS];

cpu_data_t  boot_cpu;
cpu_data_t  *secondary_cpus = NULL;
size_t      num_cpus = 1;

cpu_data_t boot_cpu_data = {
    .cpu_number = 0,
    .cpu_processor = NULL,
};

/* bootstrap processor */
processor_t bsp = {
    .state = PROCESSOR_STATE_RUNNING,
    .current_thread = NULL,
    .idle_thread = NULL,
};

void boot_cpu_init(void)
{
    sizeof(cpu_data_t);
    boot_cpu_data.cpu_processor = &bsp;
    boot_cpu = boot_cpu_data;

    cpu_data_ptr[0] = &boot_cpu;
}

void secondary_cpus_init(void)
{
}