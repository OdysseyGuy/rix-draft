#ifndef PLATFORM_H
#define PLATFORM_H

#include <x86.h>

void platform_init_console(void);
void platform_init_debug(void);

void platform_init_interrupt(void);
void platform_int_handler(x86_interrupt_frame_t* frame);

#endif /* PLATFORM_H */
