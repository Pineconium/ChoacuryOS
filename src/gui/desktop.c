/* choacury gui */
#include "desktop.h"
#include "Point.h"
#include "../drivers/ps2_keyboard.h"
#include "../drivers/ps2_mouse.h"
#include "../drivers/vga.h"

/* the mouse cursor */
static uint8_t Cursor[] = {
    // 16x2
    0b00100000, 0b00000000,
    0b00110000, 0b00000000,
    0b00101000, 0b00000000,
    0b00100100, 0b00000000,
    0b00100010, 0b00000000,
    0b00100001, 0b00000000,
    0b00100000, 0b10000000,
    0b00100000, 0b01000000,
    0b00100000, 0b00100000,
    0b00100000, 0b00010000,
    0b00100000, 0b01111000,
    0b00100110, 0b01000000,
    0b00101001, 0b00100000,
    0b00010000, 0b10010000,
    0b00000000, 0b01001000,
    0b00000000, 0b00110000
};

static uint32_t MouseCursorBuffer[16 * 16];
static Point MousePosition;

void draw_pointer(uint8_t* Cursor, Point position, u8 colour) {
    int xMax = 16;
    int yMax = 16;

    for (int y = 0; y < yMax && position.Y + y < (s32)VGA_height; y++){
        for (int x = 0; x < xMax && position.X + x < (s32)VGA_width; x++){
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((Cursor[byte] & (0b10000000 >> (x % 8)))) {
                MouseCursorBuffer[x + y * 16] = vga_getpixel(position.X + x, position.Y + y);
                vga_putpixel(position.X + x, position.Y + y, colour);
            }
        }
    }
}

void clear_pointer(uint8_t* Cursor, Point position) {
    s32 xMax = 16;
    s32 yMax = 16;

    for (s32 y = 0; y < yMax && position.Y + y < (s32)VGA_height; y++) {
        for (s32 x = 0; x < xMax && position.X + x < (s32)VGA_width; x++){
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((Cursor[byte] & (0b10000000 >> (x % 8)))) {
				vga_putpixel(position.X + x, position.Y + y, MouseCursorBuffer[x + y * 16]);
            }
        }
    }
}

void start_desktop(){
	MousePosition.X = 0;
	MousePosition.Y = 0;
	draw_pointer(Cursor, MousePosition, TC_BLACK);

	for (;;) {
		/* If ESC is pressed, exit the desktop */
		key_event_t key_event;
		ps2_get_key_event(&key_event);
		if (key_event.key == KEY_Escape) {
			break;
		}

		/* currently only handle mouse move events */
		mouse_event_t mouse_event;
		ps2_get_mouse_event(&mouse_event);
		if (mouse_event.type == MOUSE_EVENT_NONE) {
			asm volatile("hlt");
		}
		if (mouse_event.type != MOUSE_MOVE_EVENT) {
			continue;
		}

		clear_pointer(Cursor, MousePosition);
		MousePosition.X += mouse_event.move_event.rel_x;
		MousePosition.Y -= mouse_event.move_event.rel_y;
		draw_pointer(Cursor, MousePosition, TC_BLACK);
	}
}
