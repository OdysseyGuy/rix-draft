#include <pmm.h>

static list_node_t aspace_list = LIST_INITIAL_VALUE(aspace_list);

/* The kernel address space. */
vm_aspace_t kernel_aspace = {
    .base = KERNEL_ASPACE_BASE,
    .size = KERNEL_ASPACE_SIZE,
    .name = "kernel"
};

void vmm_init_preheap(void)
{
    list_init(&kernel_aspace.region_list);

    /* add the kernel address space to address space list */
    list_add(&aspace_list, &kernel_aspace.node);
}
