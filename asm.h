/* SPDX-License-Identifier: MIT */

#pragma once

#ifndef __ASSEMBLY__
#error Must be included in assembly source only
#endif

#define ELF_FUNCTION(x)                                                        \
    .global x;                                                                 \
    .type   x, STT_FUNC;                                                       \
x:

#define ELF_END_FUNCTION(x) .size x, .- x

#define ELF_DATA(x)                                                            \
    .global x;                                                                 \
    .type   x, STT_OBJECT;                                                     \
x:
