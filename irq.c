/* SPDX-License-Identifier: MIT */

#include <types.h>
#include <x86.h>

#define INT_DIV_BY_ZERO     0x0
#define INT_PAGE_FAULT      0xE

extern void
platform_int_handler(interrupt_frame_t* frame);

void
handle_div_by_zero_exception(void) {}

void
handle_page_fault_exception(interrupt_frame_t *frame)
{
    vaddr_t fault_vaddr = get_cr2();
}

void
int_handler(interrupt_frame_t *frame)
{
    uint32_t vector = frame->vector;

    switch (vector) {
    case INT_DIV_BY_ZERO:
        handle_div_by_zero_exception();
        break;
    
    case INT_PAGE_FAULT:
        handle_page_fault_exception(frame);
        break;
    
    default:
        break;

    case 0x20 ... 0xff:
        platform_int_handler(frame);
        break;
    }
}