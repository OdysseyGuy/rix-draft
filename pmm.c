/* SPDX-License-Identifier: MIT */

#include <pmm.h>
#include <balloc.h>
#include <list.h>
#include <string.h>


#define FRAME_SIZE              PAGE_SIZE
#define ZONE_FRAME_COUNT(zone)  (zone->size / FRAME_SIZE)


#define ADDRESS_BELONGS_TO_ZONE(addr, zone)                                         \
    (((addr) >= (zone)->base) && ((addr) <= ((zone)->base + (zone)->size - 1)))


#define PAGE_BELONGS_TO_ZONE(pgaddr, zone)                                          \
    ((uintptr_t)(pgaddr) >= (uintptr_t)(zone)->page_array) &&                       \
    ((uintptr_t)(pgaddr) < ((uintptr_t)(zone)->page_array + ZONE_FRAME_COUNT(zone)))


#define PAGE_INDEX_IN_ZONE(page, zone)                                              \
    (((uintptr_t)page - (uintptr_t)(zone)->page_array) / sizeof(vm_page_t))


#define PADDR_FROM_ZONE_PAGE(page, zone)                                            \
    (paddr_t)PAGE_INDEX_IN_ZONE(page, zone) * PAGE_SIZE + (zone)->base;



static list_node_t zone_list = LIST_INITIAL_VALUE(zone_list);


pmm_status_t
pmm_add_zone(_in_ pmm_zone_t *zone)
{
    if (!(zone->size > 0)) {
        return PMM_ERR_INVALID_ARGS;
    }

    /* add the zone to the zone list */
    list_add_tail(&zone_list, &zone->node);

    zone->free_count = 0;
    list_init(&zone->free_pages);

    /* allocate a page for each physical frame */
    size_t frame_count = ZONE_FRAME_COUNT(zone);
    zone->page_array = balloc(frame_count * sizeof(vm_page_t));

    /* zero the page array */
    memset(zone->page_array, 0, frame_count * sizeof(vm_page_t));

    /* add the allocated pages to free list */
    for (uint32_t page_idx = 0; page_idx < frame_count; ++page_idx) {
        vm_page_t* page = &zone->page_array[page_idx];

        list_add_tail(&zone->free_pages, &page->node);
        zone->free_count++;
    }
}


pmm_status_t
pmm_alloc_pages(
    uint32_t    *count,
    list_node_t *list)
{
    /* fast path */
    if (*count == 0) {
        return PMM_NO_ERROR;
    } else if (*count == 1) {
        *count = 0;

        vm_page_t* page;
        pmm_status_t status = pmm_alloc_page(&page);
        
        if (status == PMM_NO_ERROR) {
            /* add allocated pages to the list */
            list_add_tail(list, &page->node);
            *count = 1;
        }

        return status;
    }

    /* num pages allocated */
    uint32_t allocated = 0;

    pmm_zone_t *zone;
    list_for_each_entry(zone, &zone_list, node) {
        while ( (allocated < *count) && (zone->free_count > 0) ) {
            vm_page_t *page = list_remove_head_type(&zone->free_pages, vm_page_t, node);
            
            if (!page)
                goto done;

            page->flags |= VM_PAGE_FLAG_NONFREE;
            list_add_tail(list, &page->node);

            zone->free_count--;
            allocated++;
        }

        /* break when we have already allocated to required number of pages */
        if (allocated == *count) {
            break;
        }
    }

done:
    *count = allocated;
    return PMM_NO_ERROR;
}


pmm_status_t
pmm_alloc_page(vm_page_t **out_page)
{
    /* walk through the arena searching for free page */
    pmm_zone_t *zone;
    list_for_each_entry(zone, &zone_list, node) {
        /* allocate a page if the arena has free page */
        if (zone->free_count > 0) {
            vm_page_t *page = list_remove_head_type(&zone->free_pages, vm_page_t, node);
            if (!page)
                goto done;

            zone->free_count--;

            page->flags |= VM_PAGE_FLAG_NONFREE;
            *out_page = page;
            
            goto done;
        }
    }

done:
    return PMM_NO_ERROR;
}


size_t
pmm_alloc_range(
    paddr_t     address,
    size_t      count,
    list_node_t *list)
{
    /* make sure the address is page aligned */
    address = ROUND_DOWN(address, PAGE_SIZE);

    if (count == 0) {
        return 0;
    }
    
    size_t allocated = 0;

    /* walk through the arena, see if the physical page belongs to it */
    pmm_zone_t *zone;
    list_for_each_entry(zone, &zone_list, node) {
        while (allocated < count && ADDRESS_BELONGS_TO_ZONE(address, zone)) {
            size_t index = (address - zone->base) / PAGE_SIZE;

            /* TODO: DEBUG_ASSERT(index < a->size / PAGE_SIZE); */

            vm_page_t *page = &zone->page_array[index];
            if (page->flags & VM_PAGE_FLAG_NONFREE) {
                /* page is already allocated */
                break;
            }

            list_delete(&page->node);
            page->flags |= VM_PAGE_FLAG_NONFREE;
            list_add_tail(list, &page->node);

            zone->free_count--;
            allocated++;

            address += PAGE_SIZE;
        }

        if (allocated == count)
            break;
    }

done:
    return allocated;
}


size_t
pmm_free_pages(list_t *head)
{
    size_t count = 0;
    while(!list_is_empty(head)) {
        vm_page_t *page = list_remove_head_type(head, vm_page_t, node);

        /* find the arena this page belongs to and add the page to its free-list */
        pmm_zone_t *zone;
        list_for_each_entry(zone, &zone_list, node) {
            if (PAGE_BELONGS_TO_ZONE(page, zone)) {
                page->flags &= ~VM_PAGE_FLAG_NONFREE;
                
                list_add(&zone->free_pages, &page->node);
                zone->free_count++;
                count++;
                break;
            }
        }
    }

    return count;
}


size_t
pmm_free_page(vm_page_t *page)
{
    list_node_t list;
    list_init(&list);

    list_add(&list, &page->node);

    return pmm_free_pages(&list);
}


#if 0

pmm_status_t
pmm_alloc_contiguous(size_t count, uint8_t align_log2,
                    paddr_t *pa_out, list_node_t *list) {
    if (count == 0)
        return 0;

    /* must be atleast 4KiB */
    if (align_log2 < PAGE_SIZE_SHIFT)
        align_log2 = PAGE_SIZE_SHIFT;

    /* mutex lock */


    pmm_arena_t *arena;
    list_for_each_entry(arena, &arena_list, node) {
        if (arena->flags) {
            paddr_t rounded_base = ROUNDUP(arena->base, 1UL << align_log2);
            if (rounded_base < arena->base || rounded_base > arena->base + arena->size - 1)
                continue;

            uint32_t aligned_offset = (rounded_base - arena->base) / PAGE_SIZE;
            uint32_t start_idx = aligned_offset;

retry:
            /* keep searching while we are still within the arena (size + count < end of arena) */
            while((start_idx < ARENA_PAGE_COUNT(arena)) && (start_idx + count) <= ARENA_PAGE_COUNT(arena)) {
                vm_page_t *page = &arena->page_array[start_idx];
                for (uint32_t i = 0; i < count; ++i) {
                    if (page->flags & VM_PAGE_FLAG_NONFREE) {
                        /* failed before reaching count, break and retry */
                        start_idx = ROUNDUP(start_idx - aligned_offset + i + 1,
                            1UL << (align_log2 - PAGE_SIZE_SHIFT)) + aligned_offset;
                        goto retry;
                    }
                    page++;
                }

                /* remove the pages out of the free-list */
                for (int i = start_idx; i < start_idx + count; ++i) {
                    page = &arena->page_array[i];

                    list_delete(&page->node);
                    page->flags |= VM_PAGE_FLAG_NONFREE;
                    arena->free_count--;

                    if (list)
                        list_add_tail(list, &page->node);
                }

                if (pa_out)
                    *pa_out = arena->base + start_idx * PAGE_SIZE;

                /* mutex release */
                return NO_ERROR;
            }
        }
    }

    /* mutex release */
    return ERR_CONTIGUOUS_PAGES_NOT_FOUND;
}

#endif

void *
pmm_alloc_kpages(
    size_t      count,
    list_node_t *list)
{
    /* fast path for single page */
    if (count == 1) {
        vm_page_t* page;
        int status = pmm_alloc_page(&page);

        if (!page) {
            return NULL;
        }

        return paddr_to_kvaddr(vm_page_to_paddr(page));
    }

    /* allocate a contiguous run of physical memory */
}


size_t
pmm_free_kpages(void *ptr, uint32_t count)
{
}


void *
paddr_to_kvaddr(paddr_t pa)
{
    mmu_initial_mapping_t *map = mmu_initial_mappings;
    while (map->size > 0) {
        if ((pa >= map->phys) && (pa <= map->phys + map->size - 1)) {
            return (void *)(map->virt + (pa - map->phys));
        }
        map++;
    }
    
    return NULL;
}


paddr_t
vm_page_to_paddr(vm_page_t *page)
{
    pmm_zone_t *zone;
    list_for_each_entry(zone, &zone_list, node) {
        if (PAGE_BELONGS_TO_ZONE(page, zone)) {
            return PADDR_FROM_ZONE_PAGE(page, zone);
        }
    }
    
    return -1;
}


vm_page_t *
paddr_to_vm_page(paddr_t addr)
{
    pmm_zone_t *zone;
    list_for_each_entry(zone, &zone_list, node) {
        if (PAGE_BELONGS_TO_ZONE(addr, zone)) {
            size_t index = (addr - zone->base) / PAGE_SIZE;
            return &zone->page_array[index];
        }
    }
    
    return NULL;
}

