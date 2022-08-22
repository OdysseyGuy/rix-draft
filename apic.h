/* SPDX-License-Identifier: MIT */

#ifndef APIC_H
#define APIC_H

#define APIC_PHY_BASE   0xfee00000

void
apic_create_mapping(void);

/** @brief  Initializes the current processor's local APIC. */
void
lapic_init(void);

#endif /* !APIC_H */