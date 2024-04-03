#pragma once

#include "types.h"

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

void vga_set_char(u32 x, u32 y, u8 ch, u8 color);
void vga_move_cursor(u32 x, u32 y);
