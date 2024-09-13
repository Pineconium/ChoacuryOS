#include "ports.h"

u8 port_byte_in (u16 port) {
    u8 result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void port_byte_out (u16 port, u8 data) {
    __asm__ __volatile__("out %%al, %%dx" : : "a" (data), "d" (port));
}

u16 port_word_in (u16 port) {
    u16 result;
    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void port_word_out (u16 port, u16 data) {
    __asm__ __volatile__("out %%ax, %%dx" : : "a" (data), "d" (port));
}

u32 port_dword_in (u16 port) {
    u32 result;
    asm volatile("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void port_dword_out (u16 port, u32 data) {
    asm volatile ("outl %0, %1" : : "a"(data), "Nd"(port));
}