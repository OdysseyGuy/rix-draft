/* SPDX-License-Identifier: MIT */

#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct vc_info vc_info_t;

typedef enum con_scroll_dir { SCROLL_DIR_UP, SCROLL_DIR_DOWN } con_scroll_dir_t;

/**
 * Console font.
 */
typedef struct console_font {
    uint32_t  width, height;
    uint32_t  chcount;
    uint32_t *data;
} console_font_t;

/**
 * Console operation callbacks.
 */
typedef struct console_ops {
    void (*init)(vc_info_t *vc_info);
    void (*clear)(vc_info_t *vc_info, uint32_t sx, uint32_t sy, uint32_t height,
                  uint32_t width);

    /* put char */
    void (*putc)(vc_info_t *vc_info, uint8_t ch, uint32_t xx, uint32_t yy);

    /* scrolling */
    void (*scroll)(vc_info_t *vc_info, uint32_t top, uint32_t bottom,
                   con_scroll_dir_t dir, uint32_t lines);

    /* cursor */
    void (*hide_cursor)(vc_info_t *vc_info);
    void (*show_cursor)(vc_info_t *vc_info);

    /* font */
    void (*set_font)(vc_info_t *vc_info, console_font_t *font);
    void (*get_font)(vc_info_t *vc_info, console_font_t *font);
} console_ops_t;

typedef struct console {
    uint8_t name[16]; /* Console name. */

    /* console operations */
    void (*write)(struct console *con, const uint8_t *text, size_t size);
    void (*read)(struct console *con);
    void (*setup)(struct console *con);
    void (*exit)(struct console *con);

    struct console *next;
} console_t;

void register_console(console_t *con);
void unregister_console(console_t *con);
