/* SPDX-License-Identifier: MIT */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#define _in_
#define _out_

int
printf(
    _in_ const uint8_t  *fmt,
    ...);

int
sprintf(
    _out_ uint8_t       *str,
    _in_ const uint8_t  *fmt,
    ...);

int
snprintf(
    _out_ uint8_t       *str,
    _in_ size_t         size,
    _in_ const uint8_t  *fmt,
    ...);

int
vprintf(
    _in_ const uint8_t  *fmt,
    _in_ va_list        args);

int
vsprintf(
    _out_ uint8_t       *str,
    _in_ const uint8_t  *fmt,
    _in_ va_list        args);

int
vsnprintf(
    _out_ uint8_t       *str,
    _in_ size_t         size,
    _in_ const uint8_t  *fmt,
    _in_ va_list        args);
