/* SPDX-License-Identifier: MIT */

#include "stdio.h"
#include <compiler.h>
#include <stddef.h>
#include <stdbool.h>

#define INT32_MAX     0x7fffffff
#define UINT32_MAX    0xffffffff

#define isdigit(c)  (((c) >= '0') && ((c) <= '9'))
#define chtod(c)    ((c) - '0')

#define CAPS    0x1
#define LEFT    0x2
#define SIGN    0x4
#define SPACE   0x8
#define ALT     0x10
#define PLUS    0x20

_Static_assert(CAPS == 1);

enum number_type {
    FORMAT_TYPE_NONE = 0,
    FORMAT_TYPE_LONG_LONG,
    FORMAT_TYPE_LONG,
    FORMAT_TYPE_CHAR,
    FORMAT_TYPE_SHORT,
    FORMAT_TYPE_INT,
    FORMAT_TYPE_SIZE_T,
};

int
__printf_internal(
    const uint8_t  *fmt,
    va_list         argp,
    void          (*putc)(uint8_t c, void *arg),
    void           *arg)
{
    uint8_t         c;
    uint64_t        n;
    uint32_t        flags;
    uint8_t         ntype;
    uint8_t         padc;
    uint32_t        precision;
    uint32_t        base;
    uint32_t        width;
    int             printed = 0;

    while ((c = *fmt) != '\0') {
        if (c != '%') {
            putc(c, arg);
            fmt++;
            printed++;
            continue;
        }

        /* consume '%' */
        c = *fmt++;

        padc = ' ';
        width = 0;
        precision = -1;
        base = 10;
        ntype = FORMAT_TYPE_NONE;

        while (1) {
            bool found = true;

            ++fmt;

            switch (*fmt) {
            case '-': flags |= LEFT;    break;
            case '+': flags |= PLUS;    break;
            case ' ': flags |= SPACE;   break;
            case '#': flags |= ALT;     break;
            default:  found = false;
            }

            if (!found) {
                break;
            }
        }

        if (c == '0') {
            padc = '0';
            c = *fmt++;
        }

        /* get the field width */
        if (isdigit(c)) {
            while (isdigit(c)) {
                width = 10 * width + chtod(c);
                c = *fmt++;
            }
        } else if (c == '*') {
            width = va_arg(argp, int);
            c = *fmt++;
            if (width < 0) {
                flags &= ~LEFT;
                width = -width;
            }
        }

        /* get the precision */
        if (c == '.') {
            c = *fmt++;
            if (isdigit(c)) {
                precision = 0;
                while (isdigit(c)) {
                    precision = 10 * precision + chtod(c);
                    c = *fmt++;
                }
            } else if (c == '*') {
                precision = va_arg(argp, int);
                c = *fmt++;
            }
        }

        if (c == 'l') {
            c = *fmt++;
            ntype = FORMAT_TYPE_LONG;
            if (c == 'l') {
                ntype = FORMAT_TYPE_LONG_LONG;
                c = *fmt++;
            }
        } else if (c == 'h') {
            c = *fmt++;
            ntype = FORMAT_TYPE_SHORT;
            if (c == 'h') {
                ntype = FORMAT_TYPE_CHAR;
                c = *fmt++;
            }
        } else if (c == 'q' || c == 'L') {
            ntype = FORMAT_TYPE_LONG;
            c = *fmt++;
        }

        if (c == 'z' || c == 'Z') {
            ntype = FORMAT_TYPE_SIZE_T;
        }

        switch (c) {
        case 'c':
            c = (uint8_t)va_arg(argp, int);
            (*putc)(c, arg);
            printed++;
            break;

        case 's':
        {
            const uint8_t *s = va_arg(argp, const uint8_t *);
            const uint8_t *ssaved;

            bool ladjust = (flags & LEFT);

            if (precision == 1) {
                precision = INT32_MAX;
            }

            if (s == NULL) {
                s = "<null>";
            }

            if (width > 0 && !ladjust) {
                n = 0;
                ssaved = s;

                for (; *s != '\0' && n < precision; s++) {
                    n++;
                }
                
                s = ssaved;

                while (n < width) {
                    (*putc)(' ', arg);
                    n++;
                    printed++;
                }
            }

            n = 0;

            while ((n < precision) && (!((width > 0) && (n >= width)))) {
                if (*s == '\0') {
					break;
				}
				(*putc)(*s++, arg);
                n++;
                printed++;
            }

            if (n < width && ladjust) {
				while (n < width) {
					(*putc)(' ', arg);
					n++;
                    printed++;
				}
			}
        }

        case 'o':
            __fallthrough;
        case 'O':
            base = 8;

        case 'i':
            __fallthrough;

        case 'd':
            
            flags |= SIGN;
            break;

        case 'U':
            flags |= CAPS;
            __fallthrough;

        case 'u':
            
            break;

        case 'p':
            flags |= ALT;
            __fallthrough;

        case 'X':
            flags |= CAPS;
            base = 16;
            __fallthrough;

        case 'x':
            if (flags & ALT) {
                (*putc)(c, arg);
            }
            break;

print_signed:
            if (ntype == FORMAT_TYPE_LONG_LONG) {
                n = va_arg(argp, long long);
            } else if (ntype == FORMAT_TYPE_LONG) {
                n = va_arg(argp, long);
            } else {
                n = va_arg(argp, int);
            }

            switch (ntype) {
            case FORMAT_TYPE_CHAR:
                n = (int8_t)n;
                break;
            case FORMAT_TYPE_SHORT:
                n = (int16_t)n;
                break;
            default:
                break;
            }


print_unsigned:
            if (ntype == FORMAT_TYPE_LONG_LONG) {
                n = va_arg(argp, unsigned long long);
            } else if (ntype == FORMAT_TYPE_LONG) {
                n = va_arg(argp, unsigned long);
            } else {
                n = va_arg(argp, unsigned int);
            }

            switch (ntype) {
            case FORMAT_TYPE_CHAR:
                n = (uint8_t)n;
                break;
            case FORMAT_TYPE_SHORT:
                n = (uint16_t)n;
                break;
            default:
                break;
            }

        default:
            (*putc)(c, arg);
            break;
        }
    }

    return printed;
}


int
printf(const uint8_t *fmt, ...)
{
    va_list ap;
    int i;

    va_start(ap, fmt);
    i = vprintf(fmt, ap);
    va_end(ap);

    return i;
}

typedef struct snprintf_arg {
    uint8_t    *data;
    size_t      size;
    size_t      pos;
} snprintf_arg_t;


static void
vsnprintf_internal(uint8_t c, void *arg)
{
    snprintf_arg_t *const args = arg;

    if (args->pos < args->size - 1) {
        args->data[args->pos++] = c;
    }
}

int
vsnprintf(uint8_t *str, size_t size, const uint8_t *fmt, va_list argp)
{
    int ret;
    snprintf_arg_t info;

    info.data = str;
    info.size = size;
    info.pos = 0;

    ret = __printf_internal(fmt, argp, &vsnprintf_internal, (void *)&info);

    if (info.pos < info.size) {
        info.data[info.pos] = '\0';
    }

    return ret;
}

int
vprintf(const uint8_t *fmt, va_list args)
{
    /* return vsnprintf(, INT_MAX, fmt, args); */
}

int
vsprintf(uint8_t *str, const uint8_t *fmt, va_list args)
{
    return vsnprintf(str, INT32_MAX, fmt, args);
}

int
sprintf(uint8_t *str, const uint8_t *fmt, ...)
{
    va_list ap;
    int i;

    va_start(ap, fmt);
    i = vsprintf(str, fmt, ap);
    va_end(ap);

    return i;
}

int
snprintf(uint8_t *str, size_t size, const uint8_t *fmt, ...)
{
    va_list ap;
    int i;

    va_start(ap, fmt);
    i = vsnprintf(str, size, fmt, ap);
    va_end(ap);

    return i;
}
