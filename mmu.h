/* SPDX-License-Identifier: MIT */

#ifndef X86_MMU_H
#define X86_MMU_H


#define PAGE_SIZE                   4096

#define X86_PAGE_BIT_P              0x0001  /* Present                   */
#define X86_PAGE_BIT_RW             0x0002  /* Writable                  */
#define X86_PAGE_BIT_U              0x0004  /* User/Supervisor (0=s,1=u) */
#define X86_PAGE_BIT_PWT            0x0008  /* Page Write Through        */
#define X86_PAGE_BIT_PCD            0x0010  /* Page Cache Disabled       */
#define X86_PAGE_BIT_PS             0x0080  /* Page Size                 */
#define X86_PAGE_BIT_G              0x0100  /* Global                    */

#define X86_MMU_PG_FLAGS            (X86_PAGE_BIT_P | X86_PAGE_BIT_RW)

#define PML4_SHIFT                  39
#define PDP_SHIFT                   30
#define PD_SHIFT                    21
#define PT_SHIFT                    12
#define ADDR_OFFSET                 9

#define PDPT_ADDR_OFFSET            2

/* 11:0 page table entry flags bits */
#define X86_PAGE_ENTRY_FLAGS_MASK   0x0000000000000fff

/* dev manual: vol 3: section 4.5.4 */
#define X86_4KB_PAGE_FRAME          0x000ffffffffff000
#define X86_2MB_PAGE_FRAME          0x000fffffffe00000
#define X86_1GB_PAGE_FRAME          0x000fffffc0000000

#define X86_4KB_PAGE_OFFSET_MASK    0xfff
#define X86_2MB_PAGE_OFFSET_MASK    0x1fffff
#define X86_1GB_PAGE_OFFSET_MASK    0x3fffffff

/* extrating incides from vaddr */
#define VADDR_TO_PML4_INDEX(vaddr)  ((vaddr) >> PML4_SHIFT) & (0x1FF)
#define VADDR_TO_PDP_INDEX(vaddr)   ((vaddr) >> PDP_SHIFT) & (0x1FF)
#define VADDR_TO_PD_INDEX(vaddr)    ((vaddr) >> PD_SHIFT) & (0x1FF)
#define VADDR_TO_PT_INDEX(vaddr)    ((vaddr) >> PT_SHIFT) & (0x1FF)

#define NUM_PT_ENTRIES              512


#ifndef __ASSEMBLY__

#include <types.h>
#include <stdlib.h>

#define _in_
#define _out_

#define IS_PAGE_ALIGNED(addr)   IS_ALIGNED(addr, PAGE_SIZE)

#ifndef KERNEL_ASPACE_BASE
#   define KERNEL_ASPACE_BASE   0xffffff8000000000
#endif

/* Physical to kernel address space */
#define X86_P2KV(x)             ((uintptr_t)(x) + KERNEL_ASPACE_BASE)
#define X86_KV2P(x)             ((uintptr_t)(x) - KERNEL_ASPACE_BASE)


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


void
mmu_init(void);


/**
 * @brief
 * Check if virtual address is page aligned and cannonical.
 */
bool
mmu_check_vaddr(_in_ vaddr_t vaddr);

bool
mmu_check_paddr(_in_ paddr_t paddr);


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
mmu_status_t
mmu_get_mapping(
    _in_ vaddr_t        vaddr,
    _in_ addr_t         pml4,
    _out_ pt_entry_t   *last_valid_entry,
    _out_ uint64_t     *flags,
    _out_ uint32_t     *level);

/**
 * @brief
 * Walk the page table structures to see if the mapping between a 
 * virtual address and a physical exists. Also check the flags.
 */
mmu_status_t
mmu_check_mapping(
    _in_ vaddr_t        vaddr,
    _in_ addr_t         pml4,
    _in_ paddr_t        paddr);


/**
 * @brief
 * Maps a virtual address to physical address.
 */
mmu_status_t
mmu_map_addr(
    _in_ vaddr_t        vaddr,
    _in_ addr_t         pml4,
    _in_ paddr_t        paddr,
    _in_ uint64_t       mmu_flags);

mmu_status_t
mmu_unmap_addr(
    _in_ vaddr_t        vaddr,
    _in_ addr_t         pml4,
    _in_ paddr_t        paddr);


#endif /* __ASSEMBLY__ */

#endif /* X86_MMU_H */
