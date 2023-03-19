/* SPDX-License-Identifier: MIT */

#include <pmm.h>
#include <balloc.h>
#include <mmu.h>

extern int __code_start;
extern int __end;

extern void vmm_init_preheap(void);
extern mmu_status_t mmu_query(vaddr_t vaddr, paddr_t *paddr, uint32_t *flags);

static void mark_pages_in_use(vaddr_t va, size_t  len)
{
    va = ROUND_DOWN(va, PAGE_SIZE);
    len = PAGE_ALIGN(len + (va & (PAGE_SIZE - 1)));

    for (size_t offset = 0; offset < len; offset += PAGE_SIZE) {
        uint32_t flags;
        paddr_t pa;

        /* look up the physical address for the virtual address */
        mmu_status_t status = mmu_query(va, &pa, NULL);
        if (status == MMU_NO_ERROR) {
            /* once we have the physical address we need to allocate pages for
               the physical address to set those pages as non free. */
            pmm_alloc_range(pa,1, NULL);
        }
    }
}

void vm_init_preheap(void)
{
    vmm_init_preheap();

    /* mark all the kernel pages in use */
    mark_pages_in_use((uintptr_t)&__code_start, ((uintptr_t)&__end - (uintptr_t)&__code_start));

    /* mark all the pages used by the boot time allocator as used */
    if (boot_alloc_start != boot_alloc_end) {
        mark_pages_in_use(boot_alloc_start, boot_alloc_end - boot_alloc_start);
    }
}
