/* SPDX-License-Identifier: MIT */

#pragma once

#include <types.h>
#include <stdlib.h>
#include <stdbool.h>
#include <compiler.h>
#include <ptable.h>

#define IS_PAGE_ALIGNED(addr) IS_ALIGNED(addr, PAGE_SIZE)

/* Physical to kernel virtual address space */
#define X86_P2KV(x)           ((uintptr_t)(x) + KERNEL_ASPACE_BASE)
#define X86_KV2P(x)           ((uintptr_t)(x)-KERNEL_ASPACE_BASE)

typedef uint64_t pt_entry_t;

typedef enum x86_page_level {
    PL_FRAME, /* Physical Frame */
    PL_PT,    /* Page Table */
    PL_PD,    /* Page Directory */
    PL_PDP,   /* Page Directory Pointer */
    PL_PML4,  /* Page Map Level 4 */
    PL_NUM
} x86_page_level_t;

typedef enum x86_mmu_status {
    MMU_NO_ERROR,
    MMU_ERR_ENTRY_NOT_PRESENT,
    MMU_ERR_OUT_OF_MEMORY,
    MMU_ERR_INVALID_ARGS,
} x86_mmu_status_t;

/**
 * Initialize the MMU.
 */
void x86_mmu_init(void);

/**
 * Check if virtual address is page aligned and canonical.
 */
bool x86_mmu_check_vaddr(vaddr_t vaddr);

/**
 * Check if physical address is page aligned.
 */
bool x86_mmu_check_paddr(paddr_t paddr);

/**
 * Walks the page table structures.
 *
 * @param vaddr Virtual address to lookup in the page table structures.
 *
 * @param pml4 Base address of the PML4 table.
 *
 * @param last_valid_entry Last valid entry that was found while walking the
 * page tables. If the walk was successful then it would return the frame
 * address else returns the entry that was last visited.
 *
 * @param flags Flags set to the last valid entry.
 *
 * @param level Paging level of the last valid entry.
 */
x86_mmu_status_t x86_mmu_get_mapping(vaddr_t vaddr, addr_t pml4,
                                     pt_entry_t /* out */ *last_valid_entry,
                                     uint64_t /* out */   *flags,
                                     uint32_t /* out */   *level);

/**
 * Walk the page table structures to see if the mapping between a
 * virtual address and a physical exists.
 */
x86_mmu_status_t x86_mmu_check_mapping(vaddr_t vaddr, addr_t pml4,
                                       paddr_t paddr);

/**
 * Maps a virtual address to physical address.
 *
 * @param pml4 Base address of the PML4 table.
 *
 * @param paddr Phyiscal address.
 *
 * @param mmu_flags Flags to set while mapping.
 */
x86_mmu_status_t x86_mmu_map_addr(vaddr_t vaddr, addr_t pml4, paddr_t paddr,
                                  uint64_t mmu_flags);

/**
 * Remove the virtual to physical address mapping from mmu.
 */
x86_mmu_status_t x8_mmu_unmap_addr(vaddr_t vaddr, addr_t pml4, paddr_t paddr);
