/* SPDX-License-Identifier: MIT */

#ifndef VCON_H
#define VCON_H

#include <stdint.h>
#include <console/console.h>

/**
 * @brief
 * Video-console data.
 */
typedef struct vc_info {
    uint32_t    vc_col;     /* Columns */
    uint32_t    vc_rows;    /* Rows */
    uint32_t    vc_rowbytes;/* Bytes per row */
    con_ops_t   ops;        /* Console operations */
} vc_info_t;


#endif /* VCON_H */