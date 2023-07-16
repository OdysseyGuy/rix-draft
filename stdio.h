/* SPDX-License-Identifier: MIT */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <compiler.h>

int printf(const char *fmt, ...) __printf(1, 2);
int sprintf(char /* out */ *str, const char *fmt, ...);
int snprintf(char /* out */ *str, size_t size, const char *fmt, ...);

int vprintf(const char *fmt, va_list args);
int vsprintf(char /* out */ *str, const char *fmt, va_list args);
int vsnprintf(char /* out */ *str, size_t size, const char *fmt, va_list args);
