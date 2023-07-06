/* SPDX-License-Identifier: MIT */

#pragma once

#define NUM_PT_ENTRIES              512
#define PAGE_SIZE                   4096

#define X86_PAGE_BIT_P              0x0001  /* Present */
#define X86_PAGE_BIT_RW             0x0002  /* Writable */
#define X86_PAGE_BIT_U              0x0004  /* User/Supervisor (0=s,1=u) */
#define X86_PAGE_BIT_PWT            0x0008  /* Page Write Through */
#define X86_PAGE_BIT_PCD            0x0010  /* Page Cache Disabled */
#define X86_PAGE_BIT_PS             0x0080  /* Page Size */
#define X86_PAGE_BIT_G              0x0100  /* Global */

#define X86_MMU_PG_FLAGS            (X86_PAGE_BIT_P | X86_PAGE_BIT_RW)

#define X86_PML4_SHIFT              39
#define X86_PDP_SHIFT               30
#define X86_PD_SHIFT                21
#define X86_PT_SHIFT                12
#define X86_ADDR_OFFSET             9

#define X86_PDPT_ADDR_OFFSET        2

/* 11:0 page table entry flags bits */
#define X86_PAGE_ENTRY_FLAGS_MASK   0x0000000000000fff

/* dev manual: vol 3: section 4.5.4 */
#define X86_4KB_PAGE_FRAME          0x000ffffffffff000
#define X86_2MB_PAGE_FRAME          0x000fffffffe00000
#define X86_1GB_PAGE_FRAME          0x000fffffc0000000

#define X86_4KB_PAGE_OFFSET_MASK    0xfff
#define X86_2MB_PAGE_OFFSET_MASK    0x1fffff
#define X86_1GB_PAGE_OFFSET_MASK    0x3fffffff

#define VADDR_TO_PML4_INDEX(vaddr)  ((vaddr) >> X86_PML4_SHIFT) & (0x1FF)
#define VADDR_TO_PDP_INDEX(vaddr)   ((vaddr) >> X86_PDP_SHIFT) & (0x1FF)
#define VADDR_TO_PD_INDEX(vaddr)    ((vaddr) >> X86_PD_SHIFT) & (0x1FF)
#define VADDR_TO_PT_INDEX(vaddr)    ((vaddr) >> X86_PT_SHIFT) & (0x1FF)
