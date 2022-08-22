/* SPDX-License-Identifier: MIT */

#include <pmm.h>
#include "stdlib.h"
#include <multiboot2.h>
#include <init.h>

#define DEFAULT_MEMEND (16*1024*1024)

#ifndef MEMBASE
#   define MEMBASE 0x0
#endif

#ifndef KERNEL_ASPACE_BASE
#   define KERNEL_ASPACE_BASE 0xffffff8000000000
#endif

#ifndef KERNEL_VMA_BASE
#   define KERNEL_VMA_BASE 0xffffffff80000000
#endif


#define HIGHMEM_PADDR 0x100000

extern void
platform_init_console(void);

extern void
platform_init_debug(void);

extern multiboot2_info_t *multiboot_info_;


mmu_initial_mapping_t mmu_initial_mappings[] = {
    {
        .phys = MEMBASE,
        .virt = KERNEL_ASPACE_BASE,
        .size = 64ULL * 1024 * 1024 * 1024, /* 64 GiB */
        .name = "memory"
    },
    {
        .phys = MEMBASE,
        .virt = KERNEL_VMA_BASE,
        .size = 1ULL * 1024 * 1024 * 1024, /* 1 GiB */
        .name = "kernel"
    },
    { 0 }
};


typedef struct phys_zone {
    paddr_t base;
    size_t  size;
} phys_zone_t;


typedef struct boot_state {
    uint32_t    mem_lower;
    phys_zone_t mem_high_zones[10];
    size_t      num_high_zones;
} boot_state_t;


boot_state_t boot_state_;

static pmm_zone_t phy_zones[10];


void __init_func
init_boot_state(void) {
    multiboot_info_ = (multiboot2_info_t *)((uintptr_t)multiboot_info_ + KERNEL_VMA_BASE);

    multiboot2_tag_t const *tag = multiboot_info_->tags;
    multiboot2_tag_t const *end_tag = (multiboot2_tag_t *)((uintptr_t)multiboot_info_ + multiboot_info_->total_size);

    while (tag < end_tag && tag->type != MULTIBOOT2_TAG_TYPE_END) {
        switch (tag->type) {
        case MULTIBOOT2_TAG_TYPE_MMAP:
            multiboot2_mmap_entry_t *s = ((multiboot2_tag_mmap_t *)tag)->entries;
            multiboot2_mmap_entry_t *e = (multiboot2_mmap_entry_t *)((uintptr_t)tag + tag->size);

            uint32_t count = 0;
            for (multiboot2_mmap_entry_t const *mmap = s; mmap < e; mmap++) {
                /* save the low memory */
                if (!mmap->addr) {
                    boot_state_.mem_lower = mmap->len;
                }

                /* add the high memory region to available physical zones */
                if (mmap->type == MULTIBOOT2_MMAP_MEMORY_AVAILABLE && mmap->addr >= HIGHMEM_PADDR) {
                    boot_state_.mem_high_zones[count++] = (phys_zone_t) {
                        .base = mmap->addr,
                        .size = mmap->len,
                    };

                    boot_state_.num_high_zones++;
                }
            }

            break;
        }

        /* point to the next tag */
        tag = (multiboot2_tag_t const *)((uintptr_t)tag + ROUND_UP(tag->size, 8));
    }
}


static void
pmm_zone_init(void) {
    for (uint32_t i = 0; i < boot_state_.num_high_zones; ++i) {
        phy_zones[i].base = ROUND_UP(boot_state_.mem_high_zones[i].base, PAGE_SIZE);
        phy_zones[i].size = ROUND_DOWN(boot_state_.mem_high_zones[i].size, PAGE_SIZE);

        pmm_add_zone(&phy_zones[i]);
    }
}


void
platform_init(void) {
    /* setup serial debugging */
    platform_init_debug();

    /* setup the text console */
    platform_init_console();

    init_boot_state();

    pmm_zone_init();
}