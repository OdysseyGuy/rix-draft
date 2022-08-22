/* SPDX-License-Identifier: MIT */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#include <stddef.h>

#define _in_
#define _out_

typedef struct vc_info vc_info_t;


typedef enum con_scroll_dir {
    SCROLL_DIR_UP,
    SCROLL_DIR_DOWN
} con_scroll_dir_t;


/**
 * @brief
 * Console font.
 */
typedef struct con_font {
    uint32_t    width, height;
    uint32_t    chcount;
    uint32_t   *data;
} con_font_t;


/**
 * @brief
 * Console operation callbacks.
 */
typedef struct con_ops {
    void                      (*init)(
        _in_ vc_info_t         *vc_info
    );

    void                      (*clear)(
        _in_ vc_info_t         *vc_info,
        _in_ uint32_t           sx,
        _in_ uint32_t           sy,
        _in_ uint32_t           height,
        _in_ uint32_t           width
    );

    void                      (*putc)(
        _in_ vc_info_t         *vc_info,
        _in_ uint8_t            ch,
        _in_ uint32_t           xx,
        _in_ uint32_t           yy
    );

    void                      (*scroll)(
        _in_ vc_info_t         *vc_info,
        _in_ uint32_t           top,
        _in_ uint32_t           bottom,
        _in_ con_scroll_dir_t   dir,
        _in_ uint32_t           lines
    );

    void                      (*hide_cursor)(
        _in_ vc_info_t         *vc_info
    );

    void                      (*show_cursor)(
        _in_ vc_info_t         *vc_info
    );

    void                      (*set_font)(
        _in_ vc_info_t         *vc_info,
        _in_ con_font_t        *font
    );

    void                      (*get_font)(
        _in_ vc_info_t         *vc_info,
        _in_ con_font_t        *font
    );
} con_ops_t;


typedef struct console {
    uint8_t                     name[16];

    void                      (*write)(
        _in_ struct console    *con,
        _in_ const uint8_t     *text,
        _in_ size_t             size
    );
    void                      (*read)(
        _in_ struct console    *con
    );

    void                      (*setup)(
        _in_ struct console    *con
    );
    void                      (*exit)(
        _in_ struct console    *con
    );

    console_t                  *next;
} console_t;


void
register_console(_in_ console_t *con);

void
unregister_console(_in_ console_t *con);


#endif /* CONSOLE_H */