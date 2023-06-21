#ifndef IDT_H
#define IDT_H

#include <stdint.h>

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t type_attr);
void idt_install();

#endif /* IDT_H */

