/* SPDX-License-Identifier: MIT */

#include <stdint.h>
#include <x86.h>

#define PIC1            0x20        /* Master PIC */
#define PIC2            0xA0        /* Slave PIC */

#define PIC1_COMMAND    PIC1        /* Master PIC Command */
#define PIC1_DATA       (PIC1 + 1)  /* Master PIC Data */

#define PIC2_COMMAND    PIC2        /* Slave PIC Command */
#define PIC2_DATA       (PIC2 + 1)  /* Slave PIC Data */

#define PIC_EOI         0x20        /* PIC End-of-Interrupt command code */


/**
 * @brief   Remaps the PIC controllers, giving them the specified vector
 *          offsets, overriding the default configurations.
 */
static void
pic_remap(uint32_t offset1, uint32_t offset2)
{
    
}

void
pic_init(void)
{
    /* pic_remap(); */
}

void
pic_send_eoi(uint8_t irq)
{
    outp8(PIC1_COMMAND, PIC_EOI);
    
    /* if the irq came from slave then we need to send eoi
     * to both slave and the master chips */
    if (irq > 8) {
        outp8(PIC2_COMMAND, PIC_EOI);
    }

}