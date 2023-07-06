/* SPDX-License-Identifier: MIT */

#pragma once

#define APIC_PHY_BASE 0xfee00000

void apic_create_mapping(void);

/**
 * Initializes the current processor's local APIC.
 */
void lapic_init(void);
