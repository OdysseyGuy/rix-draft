/* SPDX-License-Identifier: MIT */

#pragma once

#include <list.h>
#include <types.h>
#include <stdlib.h>

#define PAGE_SIZE             4096
#define PAGE_SIZE_SHIFT       12

#define PAGE_ALIGN(addr)      ALIGN(addr, PAGE_SIZE)
#define IS_PAGE_ALIGNED(addr) IS_ALIGNED(addr, PAGE_SIZE)

typedef struct mmu_initial_mapping {
    vaddr_t     phys;
    paddr_t     virt;
    size_t      size;
    const char *name;
} mmu_initial_mapping_t;

extern mmu_initial_mapping_t mmu_initial_mappings[];

typedef struct vm_page {
    list_node_t node;
    uint8_t     flags;
} vm_page_t;

#define VM_PAGE_FLAG_NONFREE (0x1)

typedef enum pmm_status {
    PMM_NO_ERROR,
    PMM_ERR_INVALID_ARENA_SIZE,
    PMM_ERR_CONTIGUOUS_PAGES_NOT_FOUND,
    PMM_ERR_INVALID_ARGS,
} pmm_status_t;

/**
 * Holds a fixed-sized array of pages. Pages are allocated during
 * addition to arena list. Once the pages are freed they are
 * added to the free pages list.
 */
typedef struct pmm_zone {
    list_node_t node;      /* Zone list. */

    paddr_t base;          /* Base address from where allocated pages starts. */
    size_t  size;          /* Total size of the zone. */

    vm_page_t *page_array; /* Array of pages allocated by this zone. */

    size_t free_count;     /* Count of free pages. */
    list_t free_pages;     /* Free page list. */
} pmm_zone_t;

pmm_status_t pmm_add_zone(pmm_zone_t *zone);

/**
 * Allocates count non-contiguous pages of physical memory.
 *
 * @param count Count of pages to allocate.
 *
 * @param list List of pages that were allocated.
 */
pmm_status_t pmm_alloc_pages(uint32_t /* in/out */ *count,
                             /* out */ list_t      *list);

/**
 * Allocates a single page of physical memory.
 *
 * @param page Page allocated.
 */
pmm_status_t pmm_alloc_page(vm_page_t /* out */ **page);

/**
 * Start allocating a range of pages starting from the given address.
 *
 * @param paddr Starting physical address of the range to be allocated.
 *
 * @param count Number of pages to allocate.
 *
 * @param list List of the allocated pages.
 *
 * @returns Number of pages allocated.
 */
size_t pmm_alloc_range(paddr_t paddr, size_t count, list_t /* out */ *list);

/**
 * Allocate a run of contiguous pages, aligned on log2 byte boundary (0-31)
 *
 * @param count Number of pages to allocate.
 *
 * @param pa If the optional physical address pointer is passed, return the
 * address.
 *
 * @param list If the optional list is passed, append the allocate page
 * structures to the tail of the list.
 */
pmm_status_t pmm_alloc_contiguous(size_t count, uint8_t align_log2,
                                  paddr_t /* out */ *pa,
                                  list_t /* out */  *list);

/**
 * Frees pages in the given list.
 *
 * @returns Count of pages freed.
 */
size_t pmm_free_pages(list_t *pages);
size_t pmm_free_page(vm_page_t *page);

/**
 * Allocate pages from the kernel virtual address space.
 */
void  *pmm_alloc_kpages(size_t count, list_t /* out */ *list);
size_t pmm_free_kpages(void *ptr, uint32_t count);

/**
 * Physical address to virtual address.
 */
void *paddr_to_kvaddr(paddr_t pa);

/**
 * Virtual address to physical address.
 */
paddr_t vaddr_to_paddr(void *vaddr);

paddr_t    vm_page_to_paddr(vm_page_t *page);
vm_page_t *paddr_to_vm_page(paddr_t addr);

typedef struct vmm_region {
    list_node_t node;
} vmm_region_t;

/**
 * A virtual address space.
 */
typedef struct vm_aspace {
    list_node_t node;
    uint8_t     name[32];

    uint32_t flags;

    vaddr_t base;
    size_t  size;

    list_node_t region_list;
} vm_aspace_t;

void         vmm_init_preheap(void);
vm_aspace_t *vaddr_to_vm_aspace(vaddr_t addr);

/**
 * Allocates a physical page and maps it to the virtual page.
 *
 * @param aspace Address space to allocate the page from.
 *
 * @param name
 *
 * @param count Number of pages to allocate.
 *
 * @param paddr @param ptr
 *
 * @return pmm_status_t
 */
pmm_status_t vmm_alloc_physical(vm_aspace_t *aspace, const uint8_t *name,
                                size_t count, paddr_t paddr,
                                void /* in/out */ **ptr);
