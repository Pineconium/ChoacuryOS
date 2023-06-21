#ifndef GDT_H
#define GDT_H

#include <stdint.h>

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);
void gdt_install();

#endif /* GDT_H */

