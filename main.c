/* SPDX-License-Identifier: MIT */

#include <stdint.h>
#include <kheap.h>
#include <compiler.h>
#include <init.h>
#include <stdio.h>
#include <x86.h>
#include <thread.h>

extern void arch_init(void);
extern void platform_init(void);
extern void vm_init_preheap(void);

int __noreturn kmain(void)
{
    /* thread_init_early(); */
    init_hooks_init();

    kernel_init_upto(INIT_STAGE_VM);

    arch_init();

    platform_init();

    while (1) {}
}
