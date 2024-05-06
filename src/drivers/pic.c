#include "idt.h"
#include "pic.h"
#include "ports.h"

#define PIC_PORT_CMD_MASTER  0x20
#define PIC_PORT_DATA_MASTER 0x21
#define PIC_PORT_CMD_SLAVE   0xA0
#define PIC_PORT_DATA_SLAVE  0xA1

#define ICW1_ICW4       0x01 /* Indicates that ICW4 will be present */
#define ICW1_SINGLE     0x02 /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08 /* Level triggered (edge) mode */
#define ICW1_INIT       0x10 /* Initialization - required! */

#define ICW4_8086       0x01 /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02 /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08 /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM       0x10 /* Special fully nested (not) */

#define PIC_CMD_ISR 0x0B
#define PIC_CMD_EOI 0x20

void pic_init() {
    /* Start initialization sequence */
    port_byte_out(PIC_PORT_CMD_MASTER, ICW1_INIT | ICW1_ICW4);
    port_byte_out(PIC_PORT_CMD_SLAVE,  ICW1_INIT | ICW1_ICW4);

    /* Set vector offsets */
    port_byte_out(PIC_PORT_DATA_MASTER, IRQ_BASE);
    port_byte_out(PIC_PORT_DATA_SLAVE,  IRQ_BASE + 8);

    /* Initalize slave-master relation */
    port_byte_out(PIC_PORT_DATA_MASTER, 4);
    port_byte_out(PIC_PORT_DATA_SLAVE,  2);

    /* Use mode 8086 instead of 8080 */
    port_byte_out(PIC_PORT_DATA_MASTER, ICW4_8086);
    port_byte_out(PIC_PORT_DATA_SLAVE,  ICW4_8086);

    /* Initially mask everything but slave */
    port_byte_out(PIC_PORT_DATA_MASTER, 0xFB);
    port_byte_out(PIC_PORT_DATA_SLAVE,  0xFF);
}

void pic_unmask(u8 irq) {
    u16 port;
    if (irq >= 8) {
        port = PIC_PORT_DATA_SLAVE;
        irq -= 8;
    } else {
        port = PIC_PORT_DATA_MASTER;
    }

    u8 original = port_byte_in(port);
    port_byte_out(port, original & ~(1u << irq));
}

void pic_send_eoi(u8 irq) {
    if (irq >= 8) {
        port_byte_out(PIC_PORT_CMD_SLAVE, PIC_CMD_EOI);
    }
    port_byte_out(PIC_PORT_CMD_MASTER, PIC_CMD_EOI);
}

bool pic_is_in_service(u8 irq) {
    u16 port = PIC_PORT_CMD_MASTER;
    if (irq >= 8) {
        port = PIC_PORT_CMD_SLAVE;
        irq -= 8;
    }
    port_byte_out(port, PIC_CMD_ISR);
    return port_byte_in(port) & (1 << irq);
}
