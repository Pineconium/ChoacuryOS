#pragma once

#include "types.h"
#include "utils.h"
#include "pointerctrl.h"


/* 4-Bit (max. 16) Colour Stuff */
/* All of them are rendered on a black background (0x0~) */
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

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// VGA ports, source: http://files.osdev.org/mirrors/geezer/osd/graphics/modes.c
#define VGA_AC_INDEX 0x3C0
#define VGA_AC_WRITE 0x3C0
#define VGA_AC_READ 0x3C1
#define VGA_MISC_WRITE 0x3C2
#define VGA_SEQ_INDEX 0x3C4
#define VGA_SEQ_DATA 0x3C5
#define VGA_DAC_READ_INDEX 0x3C7
#define VGA_DAC_WRITE_INDEX 0x3C8
#define VGA_DAC_DATA 0x3C9
#define VGA_MISC_READ 0x3CC
#define VGA_GC_INDEX 0x3CE
#define VGA_GC_DATA 0x3CF
#define VGA_CRTC_INDEX 0x3D4 /* 0x3B4 */
#define VGA_CRTC_DATA 0x3D5 /* 0x3B5 */
#define VGA_INSTAT_READ 0x3DA
#define VGA_NUM_SEQ_REGS 5
#define VGA_NUM_CRTC_REGS 25
#define VGA_NUM_GC_REGS 9
#define VGA_NUM_AC_REGS 21
#define VGA_NUM_REGS (1 + VGA_NUM_SEQ_REGS + VGA_NUM_CRTC_REGS + VGA_NUM_GC_REGS + VGA_NUM_AC_REGS)


/* VGA Mode 13 (256) Colours */
#define BGA_COLOUR_BLACK           0
#define BGA_COLOUR_BLUE            1
#define BGA_COLOUR_GREEN           2
#define BGA_COLOUR_CYAN            3
#define BGA_COLOUR_RED             4
#define BGA_COLOUR_MAGENTA         5
#define BGA_COLOUR_BROWN           6
#define BGA_COLOUR_LIGHT_GREY      7
#define BGA_COLOUR_DARK_GREY       8
#define BGA_COLOUR_LIGHT_BLUE      9
#define BGA_COLOUR_LIGHT_GREEN     10
#define BGA_COLOUR_LIGHT_CYAN      11
#define BGA_COLOUR_LIGHT_RED       12
#define BGA_COLOUR_LIGHT_MAGENTA   13
#define BGA_COLOUR_LIGHT_BROWN     14
#define BGA_COLOUR_WHITE           15


void vga_set_char(u32 x, u32 y, u8 ch, u8 color);
void vga_move_cursor(u32 x, u32 y);
void vga_init(unsigned char clr);
void fillrect(unsigned char sx, unsigned char sy, unsigned char ex, unsigned char ey, unsigned char clr);
void drawchar(unsigned char c, int x, int y, int fgcolor, int bgcolor);
void draw_line(int x1, int y1, int x2, int y2, unsigned char color);
void putpixel(unsigned int x, unsigned int y, unsigned char color);
void write_registers(unsigned char *regs);
void draw_pointer(uint8_t* Cursor, Point position, unsigned char colour);
void clear_pointer(uint8_t* Cursor, Point position);

