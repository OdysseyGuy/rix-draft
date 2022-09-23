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


int
kmain(void)
{
    thread_init_early();

    init_hooks_init();

    kernel_init_upto(INIT_STAGE_VM);
    
    arch_init();


    sprintf(aba, "Hello world!");

    platform_init();

    while (1) {}
}