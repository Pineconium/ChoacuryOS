#include "gdt.h"
#include "types.h"

typedef struct {
    u16 size;
    void* address;
} __attribute__((packed)) GDTR;

typedef struct {
    u16 limit0;
    u16 base0;
    u8 base1;
    u8 access;
    u8 limit1 : 4;
    u8 flags : 4;
    u8 base2;	
} __attribute__((packed)) segment_descriptor_t;

static GDTR s_gdtr;
static segment_descriptor_t s_gdt[5];

static void gdt_write_entry(u8 index, u32 base, u32 limit, u8 access, u8 flags) {
    s_gdt[index].base0 = (base >>  0) & 0xFFFF;
    s_gdt[index].base1 = (base >> 16) & 0xFF;
    s_gdt[index].base2 = (base >> 24) & 0xFF;

    s_gdt[index].limit0 = (limit >>  0) & 0xFFFF;
    s_gdt[index].limit1 = (limit >> 16) & 0x0F;

    s_gdt[index].access = access;

    s_gdt[index].flags = flags & 0x0F;
}

static void gdt_flush() {
    asm volatile("lgdt %0" :: "m"(s_gdtr));
}

void gdt_init() {
    s_gdtr.address = s_gdt;
    s_gdtr.size = sizeof(s_gdt) - 1;

    gdt_write_entry(0, 0, 0x00000, 0x00, 0x0);     // null descriptor
    gdt_write_entry(1, 0, 0xFFFFF, 0x9A, 0xC);     // kernel code
    gdt_write_entry(2, 0, 0xFFFFF, 0x92, 0xC);     // kernel data
    gdt_write_entry(3, 0, 0xFFFFF, 0xFA, 0xC);     // user code
    gdt_write_entry(4, 0, 0xFFFFF, 0xF2, 0xC);     // user data

    gdt_flush();
}
