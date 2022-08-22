/* SPDX-License-Identifier: MIT */

#ifndef _PMM_H_
#define _PMM_H_

#include <list.h>
#include <types.h>
#include "stdlib.h"

#define _in_
#define _out_

#define PAGE_SIZE       4096
#define PAGE_SIZE_SHIFT 12

#define PAGE_ALIGN(addr)        ALIGN(addr, PAGE_SIZE)
#define IS_PAGE_ALIGNED(addr)   IS_ALIGNED(addr, PAGE_SIZE)


typedef struct mmu_initial_mapping {
    vaddr_t         phys;
    paddr_t         virt;
    size_t          size;
    const uint8_t  *name;
} mmu_initial_mapping_t;


extern mmu_initial_mapping_t mmu_initial_mappings[];


/**
 * @brief
 * Per page structure
 */
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
 * @brief
 * Holds a fixed-sized array of pages. Pages are allocated during
 * addition to arena list. Once the pages are freed they are
 * added to the free pages list.
 */
typedef struct pmm_zone {
    list_node_t node;       /* Zone list. */

    paddr_t     base;       /* Base address from where allocated
                             * pages starts.
                             */
    size_t      size;       /* Total size of the zone. */

    vm_page_t  *page_array;/* Array of pages allocated by this zone. */

    size_t      free_count; /* Count of free pages. */
    list_t      free_pages; /* Free page list. */
} pmm_zone_t;


pmm_status_t
pmm_add_zone(_in_ pmm_zone_t *zone);


/**
 * @brief
 * Allocates count non-contiguous pages of physical memory.
 */
pmm_status_t
pmm_alloc_pages(
    _in_ _out_ uint32_t *count,
    _out_ list_t        *list);

/**
 * @brief
 * Allocates count non-contiguous pages of physical memory.
 */
pmm_status_t
pmm_alloc_page(_out_ vm_page_t **page);


/**
 * @brief
 * Start allocating a range of pages starting from the given address.
 * 
 * @param[in] paddr
 * Starting physical address of the range to be allocated.
 * 
 * @param[in] count
 * Number of pages to allocate.
 * 
 * @param[out] list
 * List of the allocated pages.
 * 
 * @returns
 * Number of pages allocted.
 */
size_t
pmm_alloc_range(
    _in_  paddr_t       paddr,
    _in_  size_t        count,
    _out_ list_t       *list);

/**
 * @brief
 * Allocate a run of contiguous pages,
 * aligned on log2 byte boundary (0-31)
 * 
 * @param[in] count
 * Number of pages to allocate.
 * 
 * @param[out] pa
 * If the optional physical address pointer is passed,
 * return the address.
 * 
 * @param[out] list
 * If the optional list is passed, append the allocate
 * page structures to the tail of the list.
 */
pmm_status_t
pmm_alloc_contiguous(
    _in_  size_t        count,
    _in_  uint8_t       align_log2,
    _out_ paddr_t      *pa,
    _out_ list_t       *list);


/**
 * @brief
 * Frees pages in the given list.
 * 
 * @returns
 * Count of pages freed.
 */
size_t
pmm_free_pages(_in_ list_t *pages);

size_t
pmm_free_page(_in_ vm_page_t *page);


/**
 * @brief
 * Allocate pages from the kernel area and return the pointer
 * in kernel address space.
 */
void *
pmm_alloc_kpages(
    _in_  size_t        count,
    _out_ list_t       *list);

size_t
pmm_free_kpages(
    _in_ void          *ptr,
    _in_ uint32_t       count);



/**
 * @brief
 * Physical address to virtual address.
 */
void *
paddr_to_kvaddr(_in_ paddr_t pa);


/**
 * @brief
 * Virtual address to physical address.
 */
paddr_t
vaddr_to_paddr(_in_ void *vaddr);


paddr_t
vm_page_to_paddr(_in_ vm_page_t *page);

vm_page_t *
paddr_to_vm_page(_in_ paddr_t addr);


typedef struct vmm_region {
    list_node_t node;
} vmm_region_t;


typedef struct vmm_aspace {
    list_node_t node;
    uint8_t     name[32];
    
    uint32_t    flags;
    
    vaddr_t     base;
    size_t      size;

    list_node_t region_list;
} vmm_aspace_t;


void
vmm_init_preheap(void);


vmm_aspace_t *
vaddr_to_vmm_aspace(_in_ vaddr_t addr);


pmm_status_t
vmm_alloc_physical(
    _in_ vmm_aspace_t  *aspace,
    _in_ const uint8_t *name,
    _in_ size_t         size,
    _in_ paddr_t        paddr,
    _in_ _out_ void   **ptr);


#endif /* _PMM_H_ */
