#pragma once

#define IRQ_BASE 32

typedef void(*irq_handler_t)();

void idt_register_irq_handler(int irq, irq_handler_t handler);
void idt_init();
