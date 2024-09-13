/* choacury gui */
#include "desktop.h"
#include "Point.h"
#include "bindraw.h"    // <- so I can draw in binary without melting my brain.
#include "../drivers/ps2_keyboard.h"
#include "../drivers/ps2_mouse.h"
#include "../drivers/vbe.h"

/* the mouse cursor */
static uint8_t Cursor[] = {
    /* this needs to be some 8 bit binary stuff */
    /* if you're too lazy just chatgpt it */
    
    __X_____,________,
    __XX____,________,
    __X_X___,________,
    __X__X__,________,
    __X___X_,________,
    __X____X,________,
    __X_____,X_______,
    __X_____,_X______,
    __X_____,__X_____,
    __X_____,___X____,
    __X_____,_XXXX___,
    __X__XX_,_X______,
    __X_X__X,__X_____,
    ___X____,X__X____,
    ________,_X__X___,
    ________,__XX____
};

static uint32_t MouseCursorBuffer[16 * 16];
static Point MousePosition;

void draw_pointer(uint8_t* Cursor, Point position, u8 colour) {
    int xMax = 16;
    int yMax = 16;

    for (int y = 0; y < yMax && position.Y + y < (s32)1080; y++){
        for (int x = 0; x < xMax && position.X + x < (s32)1920; x++){
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((Cursor[byte] & (0b10000000 >> (x % 8)))) {
                MouseCursorBuffer[x + y * 16] = vbe_getpixel(position.X + x, position.Y + y);
                vbe_putpixel(position.X + x, position.Y + y, colour);
            }
        }
    }
}

void clear_pointer(uint8_t* Cursor, Point position) {
    s32 xMax = 16;
    s32 yMax = 16;

    for (s32 y = 0; y < yMax && position.Y + y < (s32)1080; y++) {
        for (s32 x = 0; x < xMax && position.X + x < (s32)1920; x++){
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((Cursor[byte] & (0b10000000 >> (x % 8)))) {
				vbe_putpixel(position.X + x, position.Y + y, MouseCursorBuffer[x + y * 16]);
            }
        }
    }
}

void start_desktop(){
	MousePosition.X = 0;
	MousePosition.Y = 0;
	vbe_clear_screen(0x000000ff);
	vbe_fillrect(0, 0, 20, 20, 0x00ff0000);

	draw_pointer(Cursor, MousePosition, 0x00000000);

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

		switch (mouse_event.type)
		{
			case MOUSE_EVENT_NONE:
				/* wait for new event */
				asm volatile("hlt");
				break;
			case MOUSE_BUTTON_EVENT:
				if (mouse_event.button_event.pressed) {
					if (MousePosition.X >= 0 && MousePosition.X < 20) {
						if (MousePosition.Y >= 0 && MousePosition.Y < 20) {
							/* does something if the boxed is clicked on */
							vbe_fillrect(75, 75, 120, 90, 0x00964B00);
						}
					}
				}
				break;
			case MOUSE_MOVE_EVENT:
				clear_pointer(Cursor, MousePosition);
				MousePosition.X += mouse_event.move_event.rel_x;
				MousePosition.Y -= mouse_event.move_event.rel_y;
				draw_pointer(Cursor, MousePosition, 0x00000000);
				break;
			default:
				break;
		}
	}
}
