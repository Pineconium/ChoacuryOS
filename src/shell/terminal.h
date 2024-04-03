#pragma once

#include "../drivers/types.h"

typedef void (*set_char_t)(u32, u32, u8, u8);
typedef void (*move_cursor_t)(u32, u32);

void term_init(u32 width, u32 height, set_char_t set_char, move_cursor_t move_cursor);
void term_clear();
void term_putchar(char ch, u8 color);
void term_set_cursor(u32 x, u32 y);
void term_write(const char* message, u8 color);
