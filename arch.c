/* SPDX-License-Identifier: MIT */

#include <mmu.h>

void arch_init(void)
{
    x86_mmu_init();
}
