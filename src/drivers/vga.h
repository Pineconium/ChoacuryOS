#pragma once

#include "types.h"
#include "utils.h"


/* Colour Stuff */
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

// Register arrays, source: http://wiki.osdev.org/VGA_Hardware
unsigned char mode_320_200_256[] = {
	/* MISC*/
	0x63,
	/* SEQ */
	0x03,
	/**
    * index 0x01
    * Clocking mode register
    * 8/9 Dot Clocks
    */
	0x01,
	/**
    * Map Mask Register, 0x02
    * 0x0F = 1111
    * Enable all 4 Maps Bits 0-3
    * chain 4 mode
    */
	0x0F,
	/**
    * map select register, 0x03
    * no character map enabled
    */
	0x00,
	/**
    * memory mode register 0x04
    * enables ch4,odd/even,extended memory
    */
	0x0E,
	/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
	0xFF,
	/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF,
	/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00, 0x00
};

unsigned char mode_80x25_text[] =
{
/* MISC */
	0x67,
/* SEQ */
	0x03, 0x00, 0x03, 0x00, 0x02,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
	0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
	0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00
};

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

