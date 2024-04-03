#include "vga.h"
#include "ports.h"

/* 
VGA Stuff, aka. (Extremely Limited) Graphics.

Currently this will only work with Virtual Machines for some reason.
*/

void vga_set_char(u32 x, u32 y, u8 ch, u8 color) {
    u16* base = (u16*)0xB8000;
    base[y * VGA_WIDTH + x] = ((u16)color << 8) | ch;
}

void vga_move_cursor(u32 x, u32 y) {
	u16 pos = y * VGA_WIDTH + x;
	port_byte_out(0x3D4, 0x0F);
	port_byte_out(0x3D5, (u8)(pos & 0xFF));
	port_byte_out(0x3D4, 0x0E);
	port_byte_out(0x3D5, (u8)(pos >> 8));
}
