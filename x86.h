/* SPDX-License-Identifier: MIT */

#pragma once

/* Control Register 0 */
#define X86_CR0_WP_BIT          0x00010000 /* Write Protect */
#define X86_CR0_PG_BIT          0x80000000 /* Paging enabled */

/* Control Register 4 */
#define X86_CR4_PAE_BIT         0x00000020 /* Physical Address Extensions */
#define X86_CR4_SMEP_BIT        0x00200000 /* Supervisor Mode Execution Protection */
#define X86_CR4_SMAP_BIT        0x00400000 /* Supervisor Mode Access Prevention */

/* MSR EFER */
#define X86_IA32_MSR_EFER       0xc0000080
#define X86_IA32_MSR_EFER_LME   0x00000100 /* Long Mode Enable */
#define X86_IA32_MSR_EFER_NXE   0x00001000 /* No-Execute Enable */

#ifndef __ASSEMBLY__

#include <stdint.h>
#include <stdbool.h>
#include <compiler.h>

/**
 * @brief
 * x86-64 Interrupt stack frame.
 */
typedef struct x86_interrupt_frame {
    uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;

    uint64_t vector;
    uint64_t error_code;
} x86_interrupt_frame_t;


static inline void x86_hlt(void)
{
    __asm__ volatile("hlt");
}

static inline void x86_sti(void)
{
    __asm__ volatile("sti");
}

static inline void x86_cli(void)
{
    __asm__ volatile("cli");
}

/*
 * I/O Ports
 */

static inline void x86_outp8(uint16_t port, uint8_t data)
{
    __asm__ volatile("outb %1, %0" : : "dN"(port), "a"(data));
}

static inline void x86_outp16(uint16_t port, uint16_t data)
{
    __asm__ volatile("outw %1, %0" : : "dN"(port), "a"(data));
}

static inline void x86_outp32(uint16_t port, uint32_t data)
{
    __asm__ volatile("outl %1, %0" : : "dN"(port), "a"(data));
}

static inline uint8_t x86_inp8(uint16_t port)
{
    uint8_t rv;
    __asm__ volatile("inb %1, %0" : "=a"(rv) : "dN"(port));
    return rv;
}

static inline uint16_t x86_inp16(uint16_t port)
{
    uint16_t rv;
    __asm__ volatile("inw %1, %0" : "=a"(rv) : "dN"(port));
    return rv;
}

static inline uint32_t x86_inp32(uint16_t port)
{
    uint32_t rv;
    __asm__ volatile("inl %1, %0" : "=a"(rv) : "dN"(port));
    return rv;
}

/*
 * CPUID
 */

static inline void x86_cpuid(uint32_t leaf, uint32_t /* out */ *eax, uint32_t /* out */ *ebx,
                             uint32_t /* out */ *ecx, uint32_t /* out */ *edx)
{
    __asm__ volatile(
        "cpuid\n"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(leaf), "c"(0)
    );
}

static inline void x86_cpuid_c(uint32_t leaf, uint32_t csel, uint32_t /* out */ *eax,
                               uint32_t /* out */ *ebx, uint32_t /* out */ *ecx, uint32_t /* out */ *edx)
{
    __asm__ volatile(
        "cpuid\n"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(leaf), "c"(csel)
    );
}

/*
 * x86-specific registers
 */

static inline uint64_t x86_read_msr(uint32_t msr_id)
{
    uint64_t rv;
    uint32_t hi, lo;

    __asm__ volatile("rdmsr\n" : "=a"(lo), "=d"(hi) : "c" (msr_id));

    rv = hi;
    rv = ((uint64_t)hi << 32) | lo;
    
    return rv;
}

static inline void x86_write_msr(uint32_t msr_id, uint64_t value)
{
    uint32_t lo = (uint32_t)value;
    uint32_t hi = (uint32_t)(value >> 32);

    __asm__ volatile ("wrmsr\n" : : "a"(lo), "d"(hi), "c"(msr_id));
}


static inline uint64_t x86_get_cr0(void)
{
    uint64_t rv;
    __asm__ volatile("mov %%cr0, %0\n" : "=r"(rv));
    return rv;
}

static inline uint64_t x86_get_cr2(void)
{
    uint64_t rv;
    __asm__ volatile("mov %%cr2, %0\n" : "=r"(rv));
    return rv;
}

static inline uint64_t x86_get_cr3(void)
{
    uint64_t rv;
    __asm__ volatile("mov %%cr3, %0\n" : "=r"(rv));
    return rv;
}

static inline uint64_t x86_get_cr4(void)
{
    uint64_t rv;
    __asm__ volatile("mov %%cr4, %0\n" : "=r"(rv));
    return rv;
}

static inline void x86_set_cr0(uint64_t val)
{
    __asm__ volatile("mov %0, %%cr0\n" : : "r"(val));
}

static inline void x86_set_cr3(uint64_t val)
{
    __asm__ volatile("mov %0, %%cr3\n" : : "r"(val));
}

static inline void x86_set_cr4(uint64_t val)
{
    __asm__ volatile("mov %0, %%cr4\n" : : "r"(val));
}

static inline uint64_t x86_save_flags(void)
{
    uint64_t state;
    __asm__ volatile(
        "pushf\n"
        "pop %0\n"
        : "=rm" (state) :: "memory"
    );
    return state;
}

static inline void x86_restore_flags(uint64_t flags)
{
    __asm__ volatile(
        "push %0\n"
        "popf\n"
        :: "g" (flags) : "memory", "cc"
    );
}

static inline uint64_t x86_get_fs_offset(uint64_t offset)
{
    uint64_t ret;
    __asm__ volatile(
        "movq %%fs:%1, %0"
        : "=r" (ret) : "m" (*(uint64_t *)(offset))
    );
    return ret;
}

static inline uint64_t x86_get_gs_offset(uint64_t offset)
{
    uint64_t ret;
    __asm__ volatile(
        "movq %%gs:%1, %0"
        : "=r" (ret) : "m" (*(uint64_t *)(offset))
    );
    return ret;
}

static inline void x86_set_fs_offset(uint64_t offset, uint64_t val)
{
    __asm__ volatile(
        "movq %0, %%fs:%1"
        :: "ir" (val), "m" (*(uint64_t *)(offset)) : "memory"
    );
}

static inline void x86_set_gs_offset(uint64_t offset, uint64_t val)
{
    __asm__ volatile(
        "movq %0, %%gs:%1"
        :: "ir" (val), "m" (*(uint64_t *)(offset)) : "memory"
    );
}

#endif /* !__ASSEMBLY__ */
