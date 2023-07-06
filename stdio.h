/* SPDX-License-Identifier: MIT */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <compiler.h>

int printf(const uint8_t *fmt, ...);
int sprintf(uint8_t /* out */ *str, const uint8_t *fmt, ...);
int snprintf(uint8_t /* out */ *str, size_t size, const uint8_t *fmt, ...);

int vprintf(const uint8_t *fmt, va_list args);
int vsprintf(uint8_t /* out */ *str, const uint8_t *fmt, va_list args);
int vsnprintf(uint8_t /* out */ *str, size_t size, const uint8_t *fmt,
            va_list args);
