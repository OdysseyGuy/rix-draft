/* SPDX-License-Identifier: MIT */

#pragma once

#include <pmm.h>

typedef struct task {
    vm_aspace_t task_aspace;

    list_t      thread_list;
    uint32_t    thread_count;
} task_t;
