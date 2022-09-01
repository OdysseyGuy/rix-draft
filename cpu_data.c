/* SPDX-License-Identifier: MIT */

#include <cpu_data.h>

cpu_data_t *cpu_data_ptr[];

cpu_data_t  boot_cpu;
cpu_data_t *secondary_cpus = NULL;
size_t      num_cpus = 1;


cpu_data_t bsp_data = {
    .cpu_number = 0,
    .cpu_processor = NULL,
};

processor_t bsp = {
    .state = PROCESSOR_STATE_RUNNING,
    .current_thread = NULL,
    .idle_thread = NULL,
};


void
boot_cpu_init(void)
{
    bsp_data.cpu_processor = &bsp;
    boot_cpu = bsp_data;

    cpu_data_ptr[0] = &boot_cpu;
}

void
secondary_cpus_init(void)
{
}