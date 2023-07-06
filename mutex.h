/* SPDX-License-Identifier: MIT */

#pragma once

#include <thread.h>

typedef struct mutex {
    thread_id_t owner;
    uint32_t    count;
} mutex_t;

void mutex_init();
void mutex_destroy();

void mutex_acquire();
void mutex_release();
