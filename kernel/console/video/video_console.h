/* SPDX-License-Identifier: MIT */

#pragma once

#include <stdint.h>
#include <console/console.h>

/**
 * Video-console data.
 */
typedef struct vc_info {
    uint32_t      vc_col;      /* Columns */
    uint32_t      vc_rows;     /* Rows */
    uint32_t      vc_rowbytes; /* Bytes per row */
    console_ops_t ops;         /* Console operations */
} vc_info_t;
