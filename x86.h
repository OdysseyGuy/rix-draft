/* SPDX-License-Identifier: MIT */

#ifndef X86_H
#define X86_H

/*
 * Control Register 0
 */
#define X86_CR0_WP_BIT          0x00010000  /* Write Protect            */
#define X86_CR0_PG_BIT          0x80000000  /* Paging enabled           */

/*
 * Control Register 4
 */
#define X86_CR4_PAE_BIT         0x00000020  /* Physical Address Extensions          */
#define X86_CR4_SMEP_BIT        0x00200000  /* Supervisor Mode Execution Protection */
#define X86_CR4_SMAP_BIT        0x00400000  /* Supervisor Mode Access Prevention    */

/*
 * Model specific register
 */
#define X86_IA32_MSR_APIC       0x0000001b  /* Extended Feature Enable Register */
#define X86_IA32_MSR_EFER       0xc0000080  /* Extended Feature Enable Register */

/*
 * EFER MSR
 */
#define X86_IA32_MSR_EFER_LME   0x00000100  /* Long Mode Enable     */
#define X86_IA32_MSR_EFER_NXE   0x00001000  /* No-Execute Enable    */


#ifndef __ASSEMBLY__

#include <stdint.h>
#include <stdbool.h>


#define _in_
#define _out_


/** @brief  x86-64 Interrupt stack frame */
typedef struct interrupt_frame {
    uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;

    uint64_t vector;
    uint64_t error_code;
} interrupt_frame_t;


static inline void
hlt(void)
{
    asm volatile("hlt");
}

static inline void
sti(void)
{
    asm volatile("sti");
}

static inline void
cli(void)
{
    asm volatile("cli");
}


static inline void
outp8(
    _in_ uint16_t   port,
    _in_ uint8_t    data)
{
    asm volatile("outb %1, %0" : : "dN"(port), "a"(data));
}

static inline void
outp16(
    _in_ uint16_t port,
    _in_ uint16_t data)
{
    asm volatile("outw %1, %0" : : "dN"(port), "a"(data));
}

static inline void
outp32(
    _in_ uint16_t port,
    _in_ uint32_t data)
{
    asm volatile("outl %1, %0" : : "dN"(port), "a"(data));
}

static inline uint8_t
inp8(_in_ uint16_t port)
{
    uint8_t rv;
    asm volatile("inb %1, %0" : "=a"(rv) : "dN"(port));
    return rv;
}

static inline uint16_t
inp16(_in_ uint16_t port)
{
    uint16_t rv;
    asm volatile("inw %1, %0" : "=a"(rv) : "dN"(port));
    return rv;
}

static inline uint32_t
inp32(_in_ uint16_t port)
{
    uint32_t rv;
    asm volatile("inl %1, %0" : "=a"(rv) : "dN"(port));
    return rv;
}


static inline void
cpuid(
    _in_ uint32_t   leaf,
    _out_ uint32_t  *eax,
    _out_ uint32_t  *ebx,
    _out_ uint32_t  *ecx,
    _out_ uint32_t  *edx)
{
    asm volatile(
        "cpuid\n"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(leaf), "c"(0)
    );
}

static inline void
cpuid_c(
    _in_ uint32_t   leaf,
    _in_ uint32_t   csel,
    _out_ uint32_t  *eax,
    _out_ uint32_t  *ebx,
    _out_ uint32_t  *ecx,
    _out_ uint32_t  *edx)
{
    asm volatile(
        "cpuid\n"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(leaf), "c"(csel)
    );
}

static inline uint32_t
cpuid_get_addr_width(void)
{
    uint32_t eax, ebx, ecx, edx;
    cpuid(0x80000008, &eax, &ebx, &ecx, &edx);
    return eax;
}

static inline bool
check_smep_avaibility(void)
{
    uint32_t eax, ebx, ecx, edx; 
    cpuid(0x7, &eax, &ebx, &ecx, &edx);
    
    return ((ebx >> 6) & 0x1);
}

static inline bool
check_smap_avaibility(void)
{
    uint32_t eax, ebx, ecx, edx; 
    cpuid(0x7, &eax, &ebx, &ecx, &edx);
    
    return ((ebx >> 19) & 0x1);
}


static inline uint64_t
read_msr(_in_ uint32_t msr_id)
{
    uint64_t rv;
    uint32_t hi, lo;
    
    asm volatile("rdmsr\n" : "=a"(lo), "=d"(hi) : "c" (msr_id));
    
    rv = hi;
    rv = ((uint64_t)hi << 32) | lo;
    
    return rv;
}

static inline void
write_msr(_in_ uint32_t msr_id, _in_ uint64_t value)
{
    uint32_t lo = (uint32_t)value;
    uint32_t hi = (uint32_t)(value >> 32);
    
    asm volatile ("wrmsr\n" : : "a"(lo), "d"(hi), "c"(msr_id));
}


static inline uint64_t
get_cr0(void)
{
    uint64_t rv;
    asm volatile("mov %%cr0, %0\n" : "=r"(rv));
    return rv;
}

static inline uint64_t
get_cr2(void)
{
    uint64_t rv;
    asm volatile("mov %%cr2, %0\n" : "=r"(rv));
    return rv;
}

static inline uint64_t
get_cr3(void)
{
    uint64_t rv;
    asm volatile("mov %%cr3, %0\n" : "=r"(rv));
    return rv;
}

static inline uint64_t
get_cr4(void)
{
    uint64_t rv;
    asm volatile("mov %%cr4, %0\n" : "=r"(rv));
    return rv;
}

static inline void
set_cr0(_in_ uint64_t val)
{
    asm volatile("mov %0, %%cr0\n" : : "r"(val));
}

static inline void
set_cr3(_in_ uint64_t val)
{
    asm volatile("mov %0, %%cr3\n" : : "r"(val));
}

static inline void
set_cr4(_in_ uint64_t val)
{
    asm volatile("mov %0, %%cr4\n" : : "r"(val));
}

typedef uint64_t x86_flags_t;

static inline x86_flags_t
save_flags(void)
{
    x86_flags_t state;

    asm volatile(
        "pushf\n"
        "pop %0\n"
        : "=rm" (state)
        :: "memory"
    );

    return state;
}

static inline void
restore_flags(_in_ x86_flags_t flags)
{
    asm volatile(
        "push %0\n"
        "popf\n"
        :: "g" (flags)
        : "memory", "cc"
    );
}

#endif /* !__ASSEMBLY__ */

#endif /* X86_H */
