# SPDX-License-Identifier: MIT

#include <asm.h>

ELF_FUNCTION(cswitch)
    push  %rbp
    movq  %rbp, %rsp
    
    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12
    pushq %rbx
    pushq %rbp

    movq  %rsp, (%rdi)
    movq  (%rsi), %rsp

    popq  %rbp
    popq  %rbx
    popq  %r12
    popq  %r13
    popq  %r14
    popq  %r15

    leaveq
    retq
ELF_END_FUNCTION(cswitch)
