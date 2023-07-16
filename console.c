/* SPDX-License-Identifier: MIT */

#include <stdint.h>

#define FB       (0xb8000 + KERNEL_VMA_BASE)

#define TAB_SIZE 4

static uint8_t cur_x = 0;
static uint8_t cur_y = 0;

void put_char(uint8_t c, uint8_t fg, uint8_t bg, uint32_t x, uint32_t y)
{
    uint8_t            attrib = (bg << 4) | (fg & 0xf);
    volatile uint16_t *video = (volatile uint16_t *)FB + (cur_y * 80 + cur_x);

    switch (c) {
    case '\t':
        cur_x += TAB_SIZE;
        if (cur_x >= 80) {
            cur_x = 0;
            cur_y++;
        }
        break;

    case '\r':
        cur_x = 0;
        break;

    case '\n':
        cur_y++;
        break;

    case '\b':
        cur_x--;
        *video = ' ';
        break;

    default:
        *video = c | (attrib << 8);
        cur_x++;
        if (cur_x == 80) {
            cur_y++;
            cur_x = 0;
        }
    }
}

void put_str(char *str, uint8_t forecolor, uint8_t backcolor)
{
    while (*str != 0) {
        put_char(*str++, forecolor, backcolor, cur_x, cur_y);
    }
}

void platform_init_console(void)
{
    put_str("Rix (build 0.0.1)\n\r", 0xf, 0x0);
    put_str("Welcome to Rix kernel!", 0xf, 0x0);
}
