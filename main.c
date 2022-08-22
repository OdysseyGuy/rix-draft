/* SPDX-License-Identifier: MIT */

#include <stdint.h>
#include <kheap.h>
#include <compiler.h>
#include <init.h>
#include <stdio.h>

extern void
arch_init(void);

extern void
platform_init(void);

extern void
vm_init_preheap(void);


void *multiboot_info_ __section(".data");

uint8_t aba[400];

int
kmain(void)
{
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