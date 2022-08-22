/* SPDX-License-Identifier: MIT */

extern void mmu_init(void);

void arch_init(void) {
    mmu_init();
}