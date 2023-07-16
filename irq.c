/* SPDX-License-Identifier: MIT */

#include <types.h>
#include <x86.h>
#include <platform.h>

#define INT_DIV_BY_ZERO 0x0
#define INT_PAGE_FAULT  0xE

void x86_div_by_zero_exception_handler(void)
{
}

void x86_page_fault_exception_handler(x86_interrupt_frame_t *frame)
{
    /* vaddr_t fault_vaddr = x86_get_cr2(); */
}

void x86_int_handler(x86_interrupt_frame_t *frame)
{
    uint32_t vector = frame->vector;

    switch (vector) {
    case INT_DIV_BY_ZERO:
        x86_div_by_zero_exception_handler();
        break;

    case INT_PAGE_FAULT:
        x86_page_fault_exception_handler(frame);
        break;

    default:
        break;

    case 0x20 ... 0xff:
        platform_int_handler(frame);
        break;
    }
}
