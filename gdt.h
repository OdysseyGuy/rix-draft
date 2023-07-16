/* SPDX-License-Identifier: MIT */

#pragma once

/* 64-bit kernel mode selectors */
#define __KERNEL_CS   0x08
#define __KERNEL_DS   0x10

/* Access bits */
#define GDE_ACB_P     0x80 /* Present              */
#define GDE_ACB_DPL_2 0x40 /* Descriptor Privilege */
#define GDE_ACB_DPL_1 0x20
#define GDE_ACB_S     0x10 /* Descriptor type      */
#define GDE_ACB_E     0x08 /* Executable           */
#define GDE_ACB_DC    0x04 /* Direction/Conforming */
#define GDE_ACB_RW    0x02 /* Readable/Writeable   */
#define GDE_ACB_A     0x01 /* Accessed             */

/* Flags */
#define GDE_FLAG_G    0x08 /* Granularity          */
#define GDE_FLAG_DB   0x04 /* Size                 */
#define GDE_FLAG_L    0x02 /* Long-mode            */
