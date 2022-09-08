/* SPDX-License-Identifier: MIT */

#include <cpu_data.h>

<<<<<<< Updated upstream
cpu_data_t *cpu_data_ptr[];
=======
<<<<<<< Updated upstream
cpu_data_t *cpu_data_ptr[];
=======
#define MAX_NCPUS 64

cpu_data_t *cpu_data_ptr[MAX_NCPUS];
>>>>>>> Stashed changes
>>>>>>> Stashed changes

cpu_data_t  boot_cpu;
cpu_data_t *secondary_cpus = NULL;
size_t      num_cpus = 1;


<<<<<<< Updated upstream
cpu_data_t bsp_data = {
=======
<<<<<<< Updated upstream
cpu_data_t bsp_data = {
=======
cpu_data_t boot_cpu_data = {
>>>>>>> Stashed changes
>>>>>>> Stashed changes
    .cpu_number = 0,
    .cpu_processor = NULL,
};

processor_t bsp = {
    .state = PROCESSOR_STATE_RUNNING,
    .current_thread = NULL,
<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
>>>>>>> Stashed changes
    .idle_thread = NULL,
};


void
boot_cpu_init(void)
{
    bsp_data.cpu_processor = &bsp;
    boot_cpu = bsp_data;
<<<<<<< Updated upstream
=======
=======
};

void
boot_cpu_init(void)
{
    boot_cpu_data.cpu_processor = &bsp;
    boot_cpu = boot_cpu_data;
>>>>>>> Stashed changes
>>>>>>> Stashed changes

    cpu_data_ptr[0] = &boot_cpu;
}

void
secondary_cpus_init(void)
{
}