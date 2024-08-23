#include "terminal.h"
#include "../drivers/utils.h"
#include "../drivers/filesystem/fat.h"
// FIXME: This code should not be VGA dependant.
//        Currently there are only colors values
//        for VGA and no "general" ones.
#include "../drivers/vga.h"

#include <kernel/panic.h>
#include <memory/kmalloc.h>
#include "../shell/terminal.h"
#include <stdarg.h>


term_info_t s_term_info;
command_history_t cmd_history = {
    .current_index = 0,
    .history_count = 0,
    .history_position = -1
};
void load_command_history_from_file(FAT_filesystem_t* fs) {
    FAT_file_t* history_file = FAT_OpenAbsolute(fs, "/command_history.txt");
    if (history_file == NULL) {
        return; // No history file found
    }

    char buffer[COMMAND_MAX_LENGTH];
    size_t offset = 0;
    size_t bytes_read;

    while ((bytes_read = FAT_Read(history_file, offset, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate

        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0'; // Remove newline
        }

        add_command_to_history(buffer);
        offset += bytes_read;
    }

    FAT_Close(history_file); // Ensure the file is properly closed
}



void save_command_history_to_file(FAT_filesystem_t* fs) {
    // Open or create the file where the command history will be stored
    FAT_file_t* file = FAT_OpenAbsolute(fs, "/command_history.txt");
    if (!file) {
        // If the file does not exist, create it
        file = FAT_Open(&fs->root_file, "command_history.txt");
        if (!file) {
            // Handle the error, e.g., print an error message or return
            term_write("Error: Could not open or create command history file.\n", TC_LRED);
            return;
        }
    }

    // Write each command from the history to the file
    for (int i = 0; i < cmd_history.history_count; i++) {
        size_t len = strlen(cmd_history.commands[i]);
        FAT_Write(file, i * len, cmd_history.commands[i], len);
        FAT_Write(file, i * len + len, "\n", 1);
    }

    // Close the file
    FAT_Close(file);
}

void add_command_to_history(const char* command) {
    if (strlen(command) == 0) return;

    strcpy(cmd_history.commands[cmd_history.current_index], command);


    cmd_history.current_index = (cmd_history.current_index + 1) % COMMAND_HISTORY_SIZE;

    if (cmd_history.history_count < COMMAND_HISTORY_SIZE) {
        cmd_history.history_count++;
    }

    cmd_history.history_position = -1;
}

const char* get_previous_command() {
    if (cmd_history.history_count == 0) return NULL;

    if (cmd_history.history_position == -1) {
        cmd_history.history_position = cmd_history.current_index - 1;
    } else if (cmd_history.history_position > 0) {
        cmd_history.history_position--;
    } else {
        return NULL; // No previous command
    }

    return cmd_history.commands[cmd_history.history_position];
}

const char* get_next_command() {
    if (cmd_history.history_count == 0 || cmd_history.history_position == -1) return NULL;

    cmd_history.history_position = (cmd_history.history_position + 1) % COMMAND_HISTORY_SIZE;

    if (cmd_history.history_position == cmd_history.current_index) {
        return NULL; // No next command
    }

    return cmd_history.commands[cmd_history.history_position];
}

void term_init(u32 width, u32 height, u32 visible_height, set_char_t set_char, move_cursor_t move_cursor) {
    s_term_info.width = width;
    s_term_info.height = height;
    s_term_info.visible_height = visible_height;
    s_term_info.buffer_start_row = 0;
    s_term_info.set_char = set_char;
    s_term_info.move_cursor = move_cursor;
    s_term_info.buffer = kmalloc(width * height * sizeof(term_cell_t));
    if (s_term_info.buffer == NULL) {
        panic("Failed to allocate memory for terminal buffer");
    }
    term_clear();
}




void term_rerender_buffer() {
    u32 offset = s_term_info.buffer_start_row * s_term_info.width;
    for (u32 y = 0; y < s_term_info.visible_height; y++) {
        for (u32 x = 0; x < s_term_info.width; x++) {
            term_cell_t cell = s_term_info.buffer[offset++];
            s_term_info.set_char(x, y, cell.ch, cell.color);
        }
    }
}



void term_clear() {
    /* write all spaces to terminal buffer */
    u32 offset = 0;
    for (u32 y = 0; y < s_term_info.height; y++) {
        for (u32 x = 0; x < s_term_info.width; x++) {
            s_term_info.buffer[offset].ch = ' ';
            s_term_info.buffer[offset].color = TC_WHITE;
            offset++;
        }
    }

    /* render buffer of only spaces */
    term_rerender_buffer();

    /* move cursor to top left corner */
    s_term_info.row = 0;
    s_term_info.col = 0;
    s_term_info.move_cursor(s_term_info.col, s_term_info.row);
}
void term_resize(u32 new_width, u32 new_height) {
    term_cell_t* new_buffer = kmalloc(new_width * new_height * sizeof(term_cell_t));
    if (new_buffer == NULL) {
        panic("Failed to allocate memory for resized terminal buffer");
    }

    // Copy the old buffer to the new buffer (considering the minimum size)
    u32 min_width = (new_width < s_term_info.width) ? new_width : s_term_info.width;
    u32 min_height = (new_height < s_term_info.visible_height) ? new_height : s_term_info.visible_height;
    for (u32 y = 0; y < min_height; y++) {
        for (u32 x = 0; x < min_width; x++) {
            new_buffer[y * new_width + x] = s_term_info.buffer[y * s_term_info.width + x];
        }
    }

    // Free the old buffer and update terminal info
    kfree(s_term_info.buffer);
    s_term_info.buffer = new_buffer;
    s_term_info.width = new_width;
    s_term_info.height = new_height;
    s_term_info.visible_height = new_height;

    // Redraw the terminal
    term_rerender_buffer();
}

void term_scroll() {
    if (s_term_info.buffer_start_row < s_term_info.height - s_term_info.visible_height) {
        s_term_info.buffer_start_row++;
    }

    // Re-render the visible portion of the buffer
    term_rerender_buffer();
}






void term_putchar_no_cursor_update(char ch, u8 color) {
    switch (ch) {
        case '\b':
            if (s_term_info.col > 0) {
                s_term_info.col--;
            }
        break;
        case '\n':
            s_term_info.col = 0;
        s_term_info.row++;
        if (s_term_info.row >= s_term_info.visible_height) {
            term_scroll();
            s_term_info.row = s_term_info.visible_height - 1;
        }
        break;
        case '\t':
            for (int i = 0; i < 4; i++) {
                term_putchar_no_cursor_update(' ', color);
            }
        break;
        default: {
            u32 offset = (s_term_info.buffer_start_row + s_term_info.row) * s_term_info.width + s_term_info.col;
            term_cell_t* cell = &s_term_info.buffer[offset];
            cell->ch = ch;
            cell->color = color;

            s_term_info.set_char(s_term_info.col, s_term_info.row, ch, color);
            s_term_info.col++;
            break;
        }
    }

    // If the column is out of bounds, move to the next line
    if (s_term_info.col >= s_term_info.width) {
        s_term_info.col = 0;
        s_term_info.row++;
        if (s_term_info.row >= s_term_info.visible_height) {
            term_scroll();
            s_term_info.row = s_term_info.visible_height - 1;
        }
    }
}


void term_putchar(char ch, u8 color) {
    term_putchar_no_cursor_update(ch, color);
    s_term_info.move_cursor(s_term_info.col, s_term_info.row);
}

void term_set_cursor(u32 x, u32 y) {
    /* update cursor column and clamp to terminal width */
    s_term_info.col = x;
    if (s_term_info.col >= s_term_info.width)
        s_term_info.col = s_term_info.width - 1;

    /* update cursor row and clamp to terminal height */
    s_term_info.row = y;
    if (s_term_info.row >= s_term_info.height)
        s_term_info.row = s_term_info.height - 1;

    /* update shown cursor position */
    s_term_info.move_cursor(s_term_info.col, s_term_info.row);
}

void term_write(const char* message, u8 color) {
    while (*message) {
        term_putchar_no_cursor_update(*message, color);
        message++;
    }

    /* update shown cursor position */
    s_term_info.move_cursor(s_term_info.col, s_term_info.row);
}
void term_write_hex(uint32_t value) {
    // Hexadecimal characters
    static const char hex_chars[] = "0123456789ABCDEF";

    // Buffer to store the resulting string
    char buffer[11]; // 0x + 8 hex digits + null terminator
    buffer[0] = '0'; // prefix '0x' to indicate hexadecimal
    buffer[1] = 'x';
    buffer[10] = '\0'; // null-terminate the string

    // Convert the integer to hexadecimal string
    for (int i = 9; i >= 2; i--) {
        buffer[i] = hex_chars[value & 0xF]; // get the last hex digit
        value >>= 4; // shift right by 4 bits (1 hex digit)
    }

    // Print the hexadecimal string to the terminal
    term_write(buffer, TC_WHITE);
}

static char get_digit_char(int digit) {
    if (digit < 10) {
        return '0' + digit;
    }
    return 'A' + (digit - 10);
}

void term_write_u32(u32 value, u8 base, u8 color) {
    if (value == 0) {
        term_putchar('0', color);
        return;
    }

    char buffer[33];
    buffer[sizeof(buffer) - 1] = 0;

    // Build the integer string in reverse order
    char* ptr = buffer + sizeof(buffer) - 1;
    while (value != 0) {
        int digit = value % base;
        *--ptr = get_digit_char(digit);
        value /= base;
    }

    // Write the integer to the terminal
    term_write(ptr, color);
}
void itoa(uintptr_t value, char* str, int base) {
    char* ptr = str, *ptr1 = str, tmp_char;
    uintptr_t tmp_value;

    if (base < 2 || base > 36) { *str = '\0'; return; }

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[tmp_value - value * base];
    } while (value);

    *ptr-- = '\0';

    // Reverse the string
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

void dprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[32];  // Adjust size if necessary
    const char* str;

    while (*format != '\0') {
        if (*format == '%' && *(format + 1) != '\0') {
            format++;  // Skip the '%'
            switch (*format) {
                case 'd':
                    itoa(va_arg(args, int), buffer, 10);
                term_write(buffer, TC_WHITE);
                break;
                case 's':
                    str = va_arg(args, const char*);
                term_write(str, TC_WHITE);
                break;
                case 'p':
                    itoa((uintptr_t)va_arg(args, void*), buffer, 16);
                term_write("0x", TC_WHITE);
                term_write(buffer, TC_WHITE);
                break;
                default:
                    term_putchar('%', TC_WHITE);
                term_putchar(*format, TC_WHITE);
                break;
            }
        } else {
            term_putchar(*format, TC_WHITE);
        }
        format++;
    }

    term_write("\n", TC_WHITE);
    va_end(args);
}

