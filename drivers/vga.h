#pragma once

#include "types.h"
#include "utils.h"

/* 4-Bit (max. 16) Colour Stuff */
/* All of them are rendered on a black background (0x0~) */
#define TC_BLACK 0x00
#define TC_BLUE 0x01
#define TC_GREEN 0x02
#define TC_CYAN 0x03
#define TC_DKRED 0x04
#define TC_MANGT 0x05
#define TC_BROWN 0x06       // <-- More of a darker yellow then brown tbh.
#define TC_WHITE 0x07       // <-- Renders Grey/White Text (Primary Text Style)
#define TC_DGREY 0x08
#define TC_LBLUE 0x09
#define TC_LIME 0x0A
#define TC_LCYAN 0x0B
#define TC_LRED 0x0C
#define TC_PINK 0x0D
#define TC_YELLO 0x0E
#define TC_BRIGHT 0x0F      // <-- This makes it actually white and not light grey as used in TC_WHITE

extern u32 VGA_width;
extern u32 VGA_height;

void vga_set_char(u32 x, u32 y, u8 ch, u8 color);
void vga_move_cursor(u32 x, u32 y);
void vga_graphics_init(u8 color);
void vga_text_init(u8 color);
void vga_fillrect(u32 sx, u32 sy, u32 ex, u32 ey, u8 color);
void vga_drawchar(unsigned char c, u32 x, u32 y, u8 fgcolor, u8 bgcolor);
void vga_drawline(u32 x1, u32 y1, u32 x2, u32 y2, u8 color);
void vga_putpixel(u32 x, u32 y, u8 color);
u8 vga_getpixel(u32 x, u32 y);
