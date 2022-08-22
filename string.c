/* SPDX-License-Identifier: MIT */

#include <string.h>
#include <stdint.h>

void *
memset(void *dest, int c, size_t count) {
    uint8_t *dst = (uint8_t *)dest;
    size_t nabytes = (-(size_t)dest) & (sizeof(size_t) - 1);
    size_t val = c & 0xff;

    /* if we have to set both aligned and non aligned bytes */
    if (count > nabytes) {
        val |= val << 8;
        val |= val << 16;
        val |= val << 32;

        /* set the non-aligned bytes memory byte-wise */
        for ( ; nabytes > 0; nabytes-- ) {
            *dst++ = c;
        }

        /* set the aligned bytes using dword */
        size_t abytes = count / sizeof(size_t);
        for ( ; abytes > 0; abytes-- ) {
            *((size_t *)dst) = val;
            dst += sizeof(size_t);
        }
    }

    for ( ; count > 0; count-- ) {
        *dst++ = c;
    }

    return dest;
}