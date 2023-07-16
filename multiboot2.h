/* SPDX-License-Identifier: MIT */

#pragma once

#include <stdint.h>

typedef struct multiboot2_tag {
    uint32_t type;
    uint32_t size;
} multiboot2_tag_t;

typedef struct multiboot2_info {
    uint32_t         total_size;
    uint32_t         reserved;
    multiboot2_tag_t tags[];
} multiboot2_info_t;

typedef struct multiboot2_mmap_entry {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t zero;
} multiboot2_mmap_entry_t;

typedef struct multiboot2_tag_mmap {
    uint32_t                type;
    uint32_t                size;
    uint32_t                entry_size;
    uint32_t                entry_version;
    multiboot2_mmap_entry_t entries[];
} multiboot2_tag_mmap_t;

enum multiboot2_mmap_memory_status {
    MULTIBOOT2_MMAP_MEMORY_AVAILABLE = 1,
    MULTIBOOT2_MMAP_MEMORY_RESERVED,
    MULTIBOOT2_MMAP_MEMORY_ACPI_RECLAIMABLE,
    MULTIBOOT2_MMAP_MEMORY_ACPI_NVS,
    MULTIBOOT2_MMAP_MEMORY_ACPI_BADRAM,
};

enum multiboot2_tag_type {
    MULTIBOOT2_TAG_TYPE_END = 0,
    MULTIBOOT2_TAG_TYPE_MMAP = 6,
};
