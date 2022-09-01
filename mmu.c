/* SPDX-License-Identifier: MIT */

#include <x86.h>
#include <mmu.h>
#include <pmm.h>
#include <string.h>


uint8_t g_paddr_width = 32;
uint8_t g_vaddr_width = 48;


bool
mmu_check_vaddr(vaddr_t vaddr)
{
    /* check if vaddr is page aligned */
    if (!IS_PAGE_ALIGNED(vaddr)) {
        return false;
    }

    uint64_t lohalf_max = (((uint64_t)1ULL << g_vaddr_width - 1) - 1);
    uint64_t hihalf_min = ~lohalf_max;


    /* check if vaddr is cannonical */
    return (vaddr > lohalf_max || vaddr < hihalf_min);
}


bool
mmu_check_paddr(paddr_t paddr)
{
    /* check if paddr is page aligned */
    if (!IS_PAGE_ALIGNED(paddr)) {
        return false;
    }

    return (paddr <= (((uint64_t) 1ULL << g_paddr_width) - 1));
}


static uint64_t
get_pfn_from_pde(uint64_t pde)
{
    uint64_t pfn;
    pfn = (pde & X86_2MB_PAGE_FRAME);
    return pfn;
}


static uint64_t
get_pfn_from_pte(uint64_t pte)
{
    uint64_t pfn;
    pfn = (pte & X86_4KB_PAGE_FRAME);
    return pfn;
}


static uint64_t
get_pml4e_from_pml4t(
    vaddr_t vaddr,
    addr_t  pml4)
{
    uint64_t *pml4t = (uint64_t*)pml4;
    uint32_t pml4e_idx = VADDR_TO_PML4_INDEX(vaddr);

    return X86_P2KV(pml4t[pml4e_idx]);
}


static uint64_t
get_pdpe_from_pdpt(
    vaddr_t vaddr,
    addr_t  pm4e)
{
    uint64_t *pdpt = (uint64_t*)pm4e;
    uint32_t pdpe_idx = VADDR_TO_PDP_INDEX(vaddr);

    return X86_P2KV(pdpt[pdpe_idx]);
}


static uint64_t
get_pde_from_pdt(
    vaddr_t vaddr,
    addr_t  pdpe)
{
    uint64_t *pdt = (uint64_t*)pdpe;
    uint32_t pde_idx = VADDR_TO_PD_INDEX(vaddr);

    return X86_P2KV(pdt[pde_idx]);
}


static uint64_t
get_pte_from_pt(
    vaddr_t vaddr,
    addr_t  pde)
{
    uint64_t *pt = (uint64_t*)pde;
    uint32_t pte_idx = VADDR_TO_PT_INDEX(vaddr);

    return X86_P2KV(pt[pte_idx]);
}


#if 0

static void
update_pte(vaddr_t vaddr, uint64_t pde, paddr_t paddr, uint64_t flags) {
    uint64_t *pt_table_ptr = (uint64_t *)(pde & X86_4KB_PAGE_FRAME);
    uint32_t pt_index = (((uint64_t)vaddr >> PT_SHIFT) & ((1UL << ADDR_OFFSET) - 1));
    
    /* set the address and present bit */
    pt_table_ptr[pt_index] = (uint64_t)paddr;
    pt_table_ptr[pt_index] |= X86_PAGE_BIT_P;

    if (flags & X86_PAGE_BIT_U) {
        pt_table_ptr[pt_index] |= X86_PAGE_BIT_U;
    }
}


static void
update_pde(vaddr_t vaddr, uint64_t pdpe, uint64_t entry, uint64_t flags) {
    uint64_t *pd_table_ptr = (uint64_t *)(pdpe & X86_4KB_PAGE_FRAME);
    uint32_t pd_index = (((uint64_t)vaddr >> PD_SHIFT) & ((1UL << ADDR_OFFSET) - 1));
    
    /* set the address and present bit */
    pd_table_ptr[pd_index] = entry;
    pd_table_ptr[pd_index] |= X86_MMU_PG_FLAGS;

    if (flags & X86_PAGE_BIT_U) {
        pd_table_ptr[pd_index] |= X86_PAGE_BIT_U;
    }
}


static void
update_pdpe(vaddr_t vaddr, uint64_t pml4e, uint64_t entry, uint64_t flags) {
    uint64_t *pdp_table_ptr = (uint64_t *)(pml4e & X86_4KB_PAGE_FRAME);
    uint32_t pdp_index = (((uint64_t)vaddr >> PDP_SHIFT) & ((1UL << ADDR_OFFSET) - 1));
    
    /* set the address and present bit */
    pdp_table_ptr[pdp_index] = entry;
    pdp_table_ptr[pdp_index] |= X86_MMU_PG_FLAGS;

    if (flags & X86_PAGE_BIT_U) {
        pdp_table_ptr[pdp_index] |= X86_PAGE_BIT_U;
    }
}


static void
update_pml4e(vaddr_t vaddr, uint64_t pml4_addr, uint64_t entry, uint64_t flags) {
    uint64_t *pml4_table_ptr = (uint64_t *)(pml4_addr);
    uint32_t pml4_index = (((uint64_t)vaddr >> PML4_SHIFT) & ((1UL << ADDR_OFFSET) - 1));
    
    /* set the address and present bit */
    pml4_table_ptr[pml4_index] = entry;
    pml4_table_ptr[pml4_index] |= X86_MMU_PG_FLAGS;

    if (flags & X86_PAGE_BIT_U) {
        pml4_table_ptr[pml4_index] |= X86_PAGE_BIT_U;
    }
}



/**
 * @brief   Convenience function to allocate a page for a page size table structure
 *          in the kernel address space.
 * 
 * @return  Page allocated.
 */
static uint64_t *
__kpage_alloc(void) {
    void *page_ptr = pmm_alloc_kpages(1, NULL);

    if (page_ptr) {
        memset(page_ptr, 0, PAGE_SIZE);
    }

    return (uint64_t *)page_ptr;
}

#endif


mmu_status_t
mmu_get_mapping(
    vaddr_t     vaddr,
    addr_t      pml4,
    uint64_t   *last_valid_entry,
    uint64_t   *out_flags,
    uint32_t   *out_lvl)
{
    uint64_t pml4e, pdpe, pde, pte;
    pml4e = get_pml4e_from_pml4t(vaddr, pml4);

    if  (!(last_valid_entry) || !(out_flags) || !(out_lvl)) {
        return MMU_ERR_INVALID_ARGS;
    }

    *out_lvl = PL_PML4;
    *last_valid_entry = pml4;
    *out_flags = 0;

    /* check if pml4 table entry present */
    if (!(pml4e & X86_PAGE_BIT_P)) {
        return MMU_ERR_ENTRY_NOT_PRESENT;
    }

    *out_lvl = PL_PDP;
    pdpe = get_pdpe_from_pdpt(vaddr, pml4e);

    if (!(pdpe & X86_PAGE_BIT_P)) {
        *last_valid_entry = pml4e;
        return MMU_ERR_ENTRY_NOT_PRESENT;
    }

    *out_lvl = PL_PD;
    pde = get_pde_from_pdt(vaddr, pdpe);

    if (!(pde & X86_PAGE_BIT_P)) {
        *last_valid_entry = pdpe;
        return MMU_ERR_ENTRY_NOT_PRESENT;
    }

    /* 2 MiB huge pages */
    if (pde & X86_PAGE_BIT_PS) {
        *last_valid_entry = get_pfn_from_pde(X86_KV2P(pde)) + ((uint64_t)vaddr & X86_2MB_PAGE_OFFSET_MASK);
        goto done;
    }

    /* 4 KiB pages */
    *out_lvl = PL_PT;
    pte = get_pte_from_pt(vaddr, pde);

    if (!(pte & X86_PAGE_BIT_P)) {
        *last_valid_entry = pde;
        return MMU_ERR_ENTRY_NOT_PRESENT;
    }

    *last_valid_entry = get_pfn_from_pte(X86_KV2P(pte)) + ((uint64_t)vaddr & X86_4KB_PAGE_OFFSET_MASK);
    *out_flags = (pte & X86_PAGE_ENTRY_FLAGS_MASK);

done:
    *out_lvl = PL_FRAME;
    return MMU_NO_ERROR;
}


#if 0

mmu_status_t
mmu_map_addr(vaddr_t vaddr, paddr_t paddr, addr_t pml4, uint64_t mmu_flags) {
    if (!mmu_check_vaddr(vaddr) && !mmu_check_paddr(paddr)) {
        return MMU_ERR_INVALID_ARGS;
    }

    mmu_status_t ret = MMU_NO_ERROR;
    
    
    uint64_t pml4e, pdpe, pde, pte;
    bool created_pdpt = 0, created_pdt = 0;

    uint64_t *entry = NULL;

    pml4e = get_pml4e_from_pml4t(vaddr, pml4);
    
    /* check for present flag bit in the entry */
    if (!(pml4e & X86_PAGE_BIT_P)) {
        /* create a new pdp table */
        entry = __kpage_alloc();
        if (entry == NULL) {
            ret = MMU_ERR_OUT_OF_MEMORY;
            goto done;
        }

        /* update the entry with present bit */
        update_pml4e(vaddr, pml4, X86_KV2P(entry), X86_PAGE_BIT_P);
        pml4e = (uint64_t)entry;
        created_pdpt = 1;
    }

    if (!created_pdpt) {
        /* in case pdpe was not created retrieve the pdp entry */
        pdpe = get_pdpe_from_pdpt(vaddr, pml4e);
    }

    /* create a pd table if the pdp table was just created 
       or the pdp entry does not exist */
    if (created_pdpt || !(pdpe & X86_PAGE_BIT_P)) {
        /* Create a new pd table */
        entry = __kpage_alloc();
        if (entry == NULL) {
            ret = MMU_ERR_OUT_OF_MEMORY;

            if (created_pdpt)
                goto clean_pdp;

            goto done;
        }

        /* update the entry with present bit */
        update_pdpe(vaddr, pml4e, X86_KV2P(entry), X86_PAGE_BIT_P);
        pdpe = (uint64_t)entry;
        created_pdt = 1;
    }

    if (!created_pdt) {
        pde = get_pdpe_from_pdpt(vaddr, pdpe);
    }

    if (created_pdt || !(pde & X86_PAGE_BIT_P)) {
        /* Create a new page table */
        bool is_2mib_page = !!(pde & X86_PAGE_BIT_PS);

        entry = __kpage_alloc();
        if (entry == NULL) {
            ret = MMU_ERR_OUT_OF_MEMORY;

            if (created_pdt)
                goto clean_pd;

            goto done;
        }

        /* if the page directory entry was pointing to a 2MiB page expand 
           it to 4KiB pages */
        if (is_2mib_page) {
            uint64_t pd_paddr = X86_KV2P(pde) & X86_4KB_PAGE_FRAME;
            uint64_t *pt_table = (uint64_t *)((uint64_t)entry & X86_4KB_PAGE_FRAME);
            uint32_t index;

            /* fill up the 512 entires with 4KiB pages */
            for (index = 0; index < NUM_PT_ENTRIES; ++index) {
                pt_table[index] = (uint64_t) pd_paddr;
                pt_table[index] |= X86_PAGE_BIT_P;

                paddr += PAGE_SIZE;
            }
        }

        /* update the entry with present bit */
        update_pde(vaddr, pdpe, X86_KV2P(entry), X86_PAGE_BIT_P);
        pde = (uint64_t)entry;
    }

    /* finally update the page table with the physical address */
    update_pte(vaddr, pde, paddr, X86_PAGE_BIT_P);
    ret = MMU_NO_ERROR;
    goto done;

clean_pd:
    if (created_pdt)
        pmm_free_page(paddr_to_vm_page(X86_P2KV(pdpe)));

clean_pdp:
    if (created_pdpt)
        pmm_free_page(paddr_to_vm_page(X86_P2KV(pml4e)));

done:
    return ret;
}

#endif

/**
 * @brief
 * Queries the MMU for the physical address mapped to the virtual
 * address. 
 */
mmu_status_t
mmu_query(
    _in_  vaddr_t   vaddr,
    _out_ paddr_t  *paddr,
    _out_ uint32_t *flags)
{
    uint64_t cr3, ret_entry, ret_flags;
    uint32_t ret_lvl;

    cr3 = get_cr3();

    pmm_status_t status = mmu_get_mapping(vaddr, X86_P2KV(cr3), &ret_entry, &ret_flags, &ret_lvl);

    if (status != MMU_NO_ERROR) {
        return status;
    }

    *paddr = (paddr_t)ret_entry;
    
    if (flags) {
        *flags = ret_flags;
    }

    return MMU_NO_ERROR;
}


void
mmu_init(void)
{
    volatile uint64_t efer_msr, cr0, cr4;

    cr0 = get_cr0();
    cr0 |= X86_CR0_WP_BIT;
    set_cr0(cr0);

    cr4 = get_cr4();
    if (check_smap_avaibility()) {
        cr4 |= X86_CR4_SMAP_BIT;
    }
    if (check_smep_avaibility()) {
        cr4 |= X86_CR4_SMEP_BIT;
    }
    set_cr4(cr4);

    /* enable no-execution in efer */
    efer_msr = read_msr(X86_IA32_MSR_EFER);
    efer_msr |= X86_IA32_MSR_EFER_NXE;
    write_msr(X86_IA32_MSR_EFER, efer_msr);

    /* query virtual and physical address sizes */
    uint32_t addr_width = cpuid_get_addr_width();
    g_paddr_width = (uint8_t)(addr_width & 0xff);
    g_vaddr_width = (uint8_t)((addr_width >> 8) & 0xff);
}