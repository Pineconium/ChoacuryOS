#include "reqheaders/stdint.h"
#include "gdt.h"

// Define the GDT entry structure
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

// Define the GDT pointer structure
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

// Define the GDT array
static gdt_entry_t gdt_entries[3];
static gdt_ptr_t gdt_ptr;

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    // Set the values of a GDT entry
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;
    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    gdt_entries[num].granularity |= granularity & 0xF0;
    gdt_entries[num].access = access;
}

void gdt_install() {
    // Install the GDT
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 3) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    // Load the GDT
    asm volatile("lgdt %0" : : "m"(gdt_ptr));

    // Set the data and code segment registers
    asm volatile(
        "movw $0x10, %%ax\n\t"
        "movw %%ax, %%ds\n\t"
        "movw %%ax, %%es\n\t"
        "movw %%ax, %%fs\n\t"
        "movw %%ax, %%gs\n\t"
        "movw %%ax, %%ss\n\t"
        "ljmp $0x08, $gdt_flush\n\t"
        "gdt_flush:\n\t"
        "retf"
        : :
        : "eax"
    );
}

