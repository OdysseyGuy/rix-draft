# SPDX-License-Identifier: MIT

#include <asm.h>
#include <gdt.h>
#include <ptable.h>
#include <x86.h>

#define BOOT_PDE_PS_FLAGS   (X86_PAGE_BIT_G | X86_PAGE_BIT_PS | X86_PAGE_BIT_RW | X86_PAGE_BIT_P)
#define BOOT_PDE_FLAGS      (X86_PAGE_BIT_RW | X86_PAGE_BIT_P)

# Physical Address
#define pa(x) (x - KERNEL_VMA_BASE)


    # Multiboot2 header section
    .section .multiboot
    .align  8
_multiboot_header:
    .long   0xe85250d6
    .long   0x0
    .long   _multiboot_header_end - _multiboot_header

    # Checksum
    .long   -(0xe85250d6 + (_multiboot_header_end - _multiboot_header))

    .word   0x0
    .word   0x0
    .long   0x8
_multiboot_header_end:


    # 32-bit kernel entrypoint
    .section .text.boot
    .code32
ELF_FUNCTION(_start)
    # multiboot_magic
    movl    %eax                    , %edi

    # multiboot2_info_t
    # movl    %ebx                    , pa(multiboot_info_)

    lgdt    pa(_pm_gdtr)
    
    # Setup the segment selectors
    movl    $__KERNEL_DS            , %eax
    movl    %eax                    , %ds
    movl    %eax                    , %es
    movl    %eax                    , %fs
    movl    %eax                    , %ss
    movl    %eax                    , %gs
  
    # Load initial kernel stack
    movl    $pa(_kstack_end)        , %esp


.Lclear_bss:
    # Clear the bss section
    mov     $pa(__bss_start)        , %edi
    mov     $pa(__bss_end)          , %ecx
    subl    %edi                    , %ecx
    xor     %eax                    , %eax
    rep     stosb


.Lenable_paging64:
    # Setup 64-bit paging

    # Enable PAE
    mov     %cr4                    , %eax
    or      $X86_CR4_PAE_BIT        , %eax
    mov     %eax                    , %cr4

    # Set the LME bit in EFER MSR
    mov     $X86_IA32_MSR_EFER      , %ecx
    rdmsr
    or      $X86_IA32_MSR_EFER_LME  , %eax
    wrmsr


.Lsetup_paging:
    # Our initial mappings:
    #
    # Kernel Address space  0xffffff8000000000 - 0xffffff9000000000 (64 GiB)
    # Kernel Code           0xffffffff80000000 - 0xffffffffc0000000 (1 GiB)
    #

    mov     $pa(boot_pml4_table)                , %eax
    mov     %eax                                , %cr3

    movl    $pa(boot_pdp_table)                 , %eax
    orl     $(X86_PAGE_BIT_RW | X86_PAGE_BIT_P) , %eax
    movl    %eax                                , pa(boot_pml4_table)

    movl    $pa(boot_pd_table)                  , %eax
    orl     $(X86_PAGE_BIT_RW | X86_PAGE_BIT_P) , %eax
    movl    %eax                                , pa(boot_pdp_table)

    # Map the higher -512 GiB
    movl    $pa(boot_pdp_high_table)            , %eax
    orl     $(X86_PAGE_BIT_RW | X86_PAGE_BIT_P) , %eax
    movl    %eax                                , pa(boot_pml4_table + 8 * 511)

    # Map the lower 1 GiB to higher -2 GiB
    movl    $pa(boot_pd_table)                  , %eax
    orl     $(X86_PAGE_BIT_RW | X86_PAGE_BIT_P) , %eax
    movl    %eax                                , pa(boot_pdp_high_table + 8 * 510)


    # Use 2 MiB pages to map first 1 GiB at 0xffffffff80000000
    mov     $pa(boot_pd_table)  , %esi
    mov     $512                , %ecx
    xor     %eax                , %eax

.Lfill_boot_pd_table:
    mov     %eax                , %ebx
    shll    $21                 , %ebx      # Address (bits (M-1):21) 
    orl     $BOOT_PDE_PS_FLAGS  , %ebx
    movl    %ebx                , (%esi)
    addl    $8                  , %esi

    inc     %eax
    loop    .Lfill_boot_pd_table


    # Set up the first 64 GiB at 0xffffff8000000000 using 2 MiB pages
    mov     $pa(boot_pde_array) , %esi
    mov     $32768              , %ecx
    xor     %eax                , %eax
    
.Lfill_boot_pde_array:
    mov     %eax                , %ebx
    shll    $21                 , %ebx      # Address (bits 31:21) 
    orl     $BOOT_PDE_PS_FLAGS  , %ebx      # Lower word of the entry
    movl    %ebx                , (%esi)

    mov     %eax                , %ebx
    shrl    $11                 , %ebx      # Address (bits (M-1):32)
    movl    %ebx                , 4(%esi)   # Higher word of the entry
    
    addl    $8                  , %esi

    inc     %eax
    loop    .Lfill_boot_pde_array


    # Point the high pdp at the linear mapping of page tables
    mov     $pa(boot_pdp_high_table), %esi
    mov     $64                     , %ecx
    movl    $pa(boot_pde_array)     , %eax
    orl     $BOOT_PDE_FLAGS         , %eax

.Lfill_boot_pdp_high_table:
    movl    %eax                    , (%esi)
    add     $8                      , %esi
    addl    $4096                   , %eax

    loop    .Lfill_boot_pdp_high_table


    # Set paging bit in cr0 and now we are in 32-bit compatibility mode.
    mov     %cr0                    , %eax
    or      $X86_CR0_PG_BIT         , %eax
    mov     %eax                    , %cr0

    # Jump to 64-bit startup code
    pushl   $__KERNEL_CS
    pushl   $pa(_start64)
    
    retf

ELF_END_FUNCTION(_start)


    # 64-bit kernel entrypoint
    .section .text.boot
    .align  8
    .code64
ELF_FUNCTION(_start64)

    movabs  $high_addr          , %rax
    jmpq    *%rax

high_addr:
    lea     _kstack_end(%rip)   , %rsp

    # Reload the higher half gdt
    lgdt    _gdtr(%rip)

    # Unmap the lower 1 GiB mapping 
    xor     %rax                , %rax
    mov     %rax                , boot_pml4_table

    # Flush TLB
    mov     %cr3                , %rax
    mov     %rax                , %cr3

    # Setup IDT
    callq   setup_idt

    # Jump to main C routine
    callq   kmain

0:
    hlt
    pause
    jmp     0b

ELF_END_FUNCTION(_start64)


    # Global Descriptor Table
    .section .rodata.gdt
    .align  8
ELF_DATA(_pm_gdtr)
    # Protected mode GDTR
    .short  _gdt_end - _gdt - 1 # length
    .int    pa(_gdt)            # address

ELF_DATA(_gdtr)
    .short  _gdt_end - _gdt - 1 # length
    .quad   _gdt                # address

    .align  8
ELF_DATA(_gdt)
    # Null segment descriptor
    .quad   0x0000000000000000

    # Kernel code selector
    .short  0xffff                                          # limit [15:0]
    .short  0x0000                                          # base1 [31:16]
    .byte   0x00                                            # base2 [39:32]
    .byte   GDE_ACB_P | GDE_ACB_S | GDE_ACB_E | GDE_ACB_RW  # access byte [47:40]
    .byte   GDE_FLAG_L << 4                                 # flags [55:52] limit [51:48]
    .byte   0x00                                            # base3 [63:56]

    # Kernel data selector
    .short  0xffff                                          # limit [15:0]
    .short  0x0000                                          # base1 [31:16]
    .byte   0x00                                            # base2 [39:32]
    .byte   GDE_ACB_P | GDE_ACB_S | GDE_ACB_RW              # access byte [47:40]
    .byte   0x00                                            # flags [55:52] limit [51:48]
    .byte   0x00                                            # base3 [63:56]
ELF_DATA(_gdt_end)


    # Kernel bootstrap stack
    .section .bss.stack
    .align  8
ELF_DATA(_kstack)
    .skip   8192
ELF_DATA(_kstack_end)


    # Bootstrap page table structures
    .section .bss.ptables
    .align  PAGE_SIZE
ELF_DATA(boot_pml4_table)
    .skip   NUM_PT_ENTRIES * 8

ELF_DATA(boot_pdp_table)
    .skip   NUM_PT_ENTRIES * 8

    # Map higher half kernel -512 GiB ... 0 GiB
ELF_DATA(boot_pdp_high_table)
    .skip   NUM_PT_ENTRIES * 8

ELF_DATA(boot_pd_table)
    .skip   NUM_PT_ENTRIES * 8

    # Huge array of 2 MiB pages Used to map 64 GiB of 
    # memory at kernel address space base (0xffffff8000000000)
ELF_DATA(boot_pde_array)
    .skip   64 * (NUM_PT_ENTRIES * 8)   # 64 x 1 GiB pdp tables
