/* choacury gui */
#include "desktop.h"
#include "../drivers/mouse.h"
#include "../drivers/vga.h"

/* the mouse cursor */
uint8_t Cursor[] = {
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

void Desktop(){
     /* make the mouse */
     // draw_pointer(Cursor, 00, 1);

     /* add drawing stuff, see VGA.H for what can be drawn */
     while(1==1);
}
