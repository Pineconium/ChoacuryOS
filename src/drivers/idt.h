#pragma once

typedef void(*irq_handler_t)();

void idt_register_irq_handler(int irq, irq_handler_t handler);
void idt_init();
