/* SPDX-License-Identifier: MIT */

#include <stdint.h>
#include <kheap.h>
#include <compiler.h>
#include <init.h>
#include <stdio.h>
#include <x86.h>
#include <thread.h>

extern void
arch_init(void);

extern void
platform_init(void);

extern void
vm_init_preheap(void);


void *multiboot_info_ __section(".data");

uint8_t aba[400];

<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
=======
>>>>>>> Stashed changes
>>>>>>> Stashed changes

void
uspace_entry(interrupt_frame_t *iframe)
{
}

<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
>>>>>>> Stashed changes
>>>>>>> Stashed changes
>>>>>>> Stashed changes
int
kmain(void)
{
    thread_init_early();

    init_hooks_init();

    kernel_init_upto(INIT_STAGE_VM);
    
    arch_init();


    sprintf(aba, "Hello world!");

    platform_init();


    // kernel_init_upto(INIT_STAGE_KPRINTF);

    // vm_init_preheap();


    // kernel_init_upto(INIT_STAGE_HEAP);

    // kheap_init();

    while (1) {}
}