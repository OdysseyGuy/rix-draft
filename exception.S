# SPDX-License-Identifier: MIT

#include <asm.h>
#include <gdt.h>

#define NUM_ISR 256

    # Interrupt service routine stub
    .section .text.isr
_isr:    
    .set    vector , 0
    .rept   NUM_ISR
        .set    isr_entry_start , .

        .if vector == 0x8 || vector >= 0xA && vector <= 0xE || vector == 0x11
            .align 16
            
            nop
            nop                 # Error code pushed by exception
            pushq   $vector
            jmp     isr_common

            .align 16
        .else
            .align 16
            
            pushq   $0          # Dummy error code
            pushq   $vector     # Interrupt vector
            jmp     isr_common

            .align 16
        .endif

        # Figure out the length of an ISR entry. We need this to
        # loop through the stub later to setup IDT.
        .set    isr_entry_len , . - isr_entry_start
        .set    vector , vector + 1 
    .endr


    # Generic interrupt service routine passes the control to x86_int_handler()
    .section .text.isr.common
isr_common:
    # Clear the direction bit
    cld

    # Save general purpose registers
    pushq   %r15
    pushq   %r14
    pushq   %r13
    pushq   %r12
    pushq   %r11
    pushq   %r10
    pushq   %r9
    pushq   %r8
    pushq   %rax
    pushq   %rcx
    pushq   %rdx
    pushq   %rbx
    pushq   %rbp
    pushq   %rsi
    pushq   %rdi

    # call the C interrupt handler
    movq    %rsp, %rdi
    call    x86_int_handler

    # Restore general purpose registers
    popq    %rdi
    popq    %rsi
    popq    %rbp
    popq    %rbx
    popq    %rdx
    popq    %rcx
    popq    %rax
    popq    %r8
    popq    %r9
    popq    %r10
    popq    %r11
    popq    %r12
    popq    %r13
    popq    %r14
    popq    %r15

    # Drop vector number and error code
    addq    $16, %rsp
    iretq


    # Interrupt descriptor table initialization routine
    .section .text.idt
ELF_FUNCTION(setup_idt)
    # Loop through the IDT setting up offsets of ISRs
    movabs  $_isr           , %rsi
    movabs  $_idt           , %rdi
    movl    $NUM_ISR        , %ecx

.Lprepare_idt:
    mov     %rsi            , %rbx      # offset [15:0]
    movw    %bx             , (%rdi)

    shr     $16             , %rbx
    movw    %bx             , 6(%rdi)   # offset [31:16]

    shr     $16             , %rbx
    movl    %ebx            , 8(%rdi)   # offset [63:32]

    add     $16             , %rdi      # next IDT entry
    add     $isr_entry_len  , %rsi      # next ISR entry

    loop    .Lprepare_idt

    # Load the IDT from IDTR
    lidt    _idtr(%rip)
    ret


    .section .rodata.idtr
    .align  8
ELF_DATA(_idtr)
    .short  _idt_end - _idt - 1 # limit
    .quad   _idt                # base


    # Interrupt descriptor table
    .section .data.idt
    .align  8
ELF_DATA(_idt)
    .set    i , 0
    .rept   NUM_ISR
        .short  0x0000      # offset
        .short  __KERNEL_CS # code selector

        .byte   0x00
        .byte   0x8e        # present, ring 0, 64-bit interrupt gate

        .short  0x0000      # offset

        .short  0x0000      # offset
        .short  0x0000      # offset

        .short  0x0000      # reserved
        .short  0x0000      # reserved

        .set    i , i + 1
    .endr
ELF_DATA(_idt_end)
