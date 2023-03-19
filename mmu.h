/* SPDX-License-Identifier: MIT */

#ifndef X86_MMU_H
#define X86_MMU_H

#include <types.h>
#include <stdlib.h>
#include <stdbool.h>
#include <compiler.h>
#include <ptable.h>


#define IS_PAGE_ALIGNED(addr) IS_ALIGNED(addr, PAGE_SIZE)

/* Physical to kernel virtual address space */
#define X86_P2KV(x) ((uintptr_t)(x) + KERNEL_ASPACE_BASE)
#define X86_KV2P(x) ((uintptr_t)(x) - KERNEL_ASPACE_BASE)

typedef uint64_t pt_entry_t;

typedef enum page_level {
    PL_FRAME,   /* Physical Frame */
    PL_PT,      /* Page Table */
    PL_PD,      /* Page Directory */
    PL_PDP,     /* Page Directory Pointer */
    PL_PML4,    /* Page Map Level 4 */
    PL_NUM
} page_level_t;

typedef enum mmu_status {
    MMU_NO_ERROR,
    MMU_ERR_ENTRY_NOT_PRESENT,
    MMU_ERR_OUT_OF_MEMORY,
    MMU_ERR_INVALID_ARGS,
} mmu_status_t;

void mmu_init(void);

/**
 * @brief
 * Check if virtual address is page aligned and canonical.
 */
bool mmu_check_vaddr(_in_ vaddr_t vaddr);

bool mmu_check_paddr(_in_ paddr_t paddr);

/**
 * @brief
 * Walks the page table structures.
 * 
 * @param vaddr
 * Virtual address to lookup in the page table structures.
 * 
 * @param pml4
 * Base address of the PML4 table.
 * 
 * @param last_valid_entry
 * Last valid entry that was found while walking the page tables.
 * If the walk was successful then it would return the frame address
 * else returns the entry that was last visited.
 * 
 * @param flags
 * Flags set to the last valid entry.
 * 
 * @param level
 * Paging level of the last valid entry.
 */
mmu_status_t mmu_get_mapping(_in_ vaddr_t vaddr, _in_ addr_t pml4,
                             _out_ pt_entry_t *last_valid_entry, _out_ uint64_t *flags,
                             _out_ uint32_t *level);

/**
 * @brief
 * Walk the page table structures to see if the mapping between a 
 * virtual address and a physical exists.
 */
mmu_status_t mmu_check_mapping(_in_ vaddr_t vaddr, _in_ addr_t pml4, _in_ paddr_t paddr);

/**
 * @brief
 * Maps a virtual address to physical address.
 * 
 * @param pml4
 * Base address of the PML4 table.
 * 
 * @param paddr
 * Phyiscal address.
 * 
 * @param mmu_flags
 * Flags to set while mapping.
 */
mmu_status_t mmu_map_addr(_in_ vaddr_t vaddr, _in_ addr_t pml4, _in_ paddr_t paddr,
                          _in_ uint64_t mmu_flags);

/**
 * @brief
 * Remove the virtual to physical address mapping from mmu.
 */
mmu_status_t mmu_unmap_addr(_in_ vaddr_t vaddr, _in_ addr_t pml4, _in_ paddr_t paddr);

#endif /* X86_MMU_H */
