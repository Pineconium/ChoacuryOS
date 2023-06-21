#include "reqheaders/stdint.h"
#include "idt.h"

// Define the IDT entry structure
typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;

// Define the IDT pointer structure
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

// Define the IDT array
static idt_entry_t idt_entries[256];
static idt_ptr_t idt_ptr;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t type_attr) {
    // Set the values of an IDT entry
    idt_entries[num].offset_low = (base & 0xFFFF);
    idt_entries[num].offset_high = (base >> 16) & 0xFFFF;
    idt_entries[num].selector = selector;
    idt_entries[num].zero = 0;
    idt_entries[num].type_attr = type_attr;
}

void idt_install() {
    // Install the IDT
    idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1;
    idt_ptr.base = (uint32_t)&idt_entries;

    // Load the IDT
    asm volatile("lidt %0" : : "m"(idt_ptr));
}

