#pragma once

#include "../drivers/types.h"
#include "../drivers/filesystem/fat.h"

#define COMMAND_HISTORY_SIZE 50
#define COMMAND_MAX_LENGTH 256

typedef void (*set_char_t)(u32, u32, u8, u8);
typedef void (*move_cursor_t)(u32, u32);
typedef struct {
    u8 ch;
    u8 color;
} term_cell_t;

typedef struct {
    u32 width;
    u32 height;
    u32 visible_height;
    u32 buffer_start_row;

    set_char_t set_char;
    move_cursor_t move_cursor;

    u32 row;
    u32 col;

    term_cell_t* buffer;
} term_info_t;
typedef struct {
    char commands[COMMAND_HISTORY_SIZE][COMMAND_MAX_LENGTH];
    int current_index;
    int history_count;
    int history_position;
} command_history_t;

extern term_info_t s_term_info;
extern command_history_t cmd_history;

void save_command_history_to_file(FAT_filesystem_t* fs);
void load_command_history_from_file(FAT_filesystem_t* fs);
void add_command_to_history(const char* command);
const char* get_previous_command();
const char* get_next_command();
void term_init(u32 width, u32 height, u32 visible_height, set_char_t set_char, move_cursor_t move_cursor);
void term_rerender_buffer();
void term_clear();
void term_putchar(char ch, u8 color);
void term_set_cursor(u32 x, u32 y);
void term_write(const char* message, u8 color);
void term_write_u32(u32 value, u8 base, u8 color);
void term_write_hex(uint32_t value);
void term_scroll();
// debug
void dprintf(const char* format, ...);
