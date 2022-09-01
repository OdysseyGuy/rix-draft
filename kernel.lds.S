/* SPDX-License-Identifier: MIT */

ENTRY(_start)
OUTPUT_FORMAT(elf64-x86-64)

KERNEL_OFFSET   = 0xffffffff80000000;
KERNEL_START    = 0x100000;

SECTIONS {
    . = KERNEL_START + KERNEL_OFFSET;

    /* Text */
    .text : AT(ADDR(.text) - KERNEL_OFFSET) {
        __code_start = .;
        *(.multiboot)
        KEEP(*(.text.boot))
        *(.text .text.*)
        __code_end = .;
    }

    /* Read-only Data */
    . = ALIGN(4096);
    .rodata : AT(ADDR(.rodata) - KERNEL_OFFSET) {
        __rodata_start = .;
        *(.rodata .rodata.*)
        __rodata_end = .;
    }

    /* Data */
    . = ALIGN(4096);
    .data : AT(ADDR(.data) - KERNEL_OFFSET) {
        __data_start = .;
        __data_end = .;
        *(.data .data.*)
    }

    /*
     * Init code and data section.
     * This section will be freed once we are done with kernel
     * initialization.
     */
    . = ALIGN(4096);
    .init.start : AT(ADDR(.init.start) - KERNEL_OFFSET) {
        __init_start = .;
    }

    . = ALIGN(4096);
    .init.text : AT(ADDR(.init.text) - KERNEL_OFFSET) {
        *(.init.text .init.text.*)
    }

    .init.data : AT(ADDR(.init.data) - KERNEL_OFFSET) {
        *(.init.data .init.data.*)

        __init_hooks_start = .;
        *(.init.hook)
        __init_hooks_end = .;
    }

    . = ALIGN(4096);
    .init.end : AT(ADDR(.init.end) - KERNEL_OFFSET) {
        __init_end = .;
    }


    /* BSS */
    . = ALIGN(4096);
    .bss : AT(ADDR(.bss) - KERNEL_OFFSET) {
        __bss_start = .;
        *(.bss.stack)
        *(.bss.ptables)
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }

    __end = .;

    /DISCARD/ : {
        *(.comment .eh_frame)
    }
}
