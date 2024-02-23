/*
    The kernel of Choacury
    I call it the Choakern! The code still needs to be finished tho...
*/

/* Includes needed for the kernel to actually work.*/
#include "../drivers/ports.h"
#include "../drivers/gdt.h"
#include "../drivers/idt.h"
#include "../drivers/pic.h"
#include "../drivers/pit.h"
#include "../drivers/ps2.h"
#include "../drivers/ps2_keymap_fi.h"   // <-- Finnish Keyboard Layout.
#include "../drivers/types.h"
#include "../drivers/vga.h"
#include "../drivers/pci.h"

/* PC Speaker Stuff */
static void startbeep(__UINT32_TYPE__ nFrequence) {
    __UINT32_TYPE__ Div;
    __UINT8_TYPE__ tmp;

    /* Sets the PIT to the desired freq */
    Div = 1193180 / nFrequence;
    port_byte_out(0x43, 0xb6);
    port_byte_out(0x42, (__UINT8_TYPE__) (Div));
    port_byte_out(0x42, (__UINT8_TYPE__) (Div >> 8));

    /* Now lets actually BEEP */
    tmp = port_byte_in(0x61);
    if (tmp != (tmp | 3)) {
        port_byte_out(0x61, tmp | 3);
    }
}

/* Make the beeper shut up */
static void mutebeep() {
    __UINT8_TYPE__ tmp = port_byte_in(0x61) & 0xFC;

    port_byte_out(0x61, tmp);
}

/* Startup Beep*/
void StartUp_Beeps() {
    startbeep(450);
    pit_sleep_ms(100);
    mutebeep();
}

/* Update the cursor */
void update_cursor(int x, int y)
{
	__UINT16_TYPE__ pos = y * 80 + x;
 
	port_byte_out(0x3D4, 0x0F);
	port_byte_out(0x3D5, (__UINT8_TYPE__) (pos & 0xFF));
	port_byte_out(0x3D4, 0x0E);
	port_byte_out(0x3D5, (__UINT8_TYPE__) ((pos >> 8) & 0xFF));
}

/* A Simple kernel written in C */
void k_main() 
{
    /* Placeholder DEFAULT cursor vars*/
    int prevX = 0;
    int prevY = 4;

    gdt_init();
    idt_init();

    update_cursor(0, 4);   // <-- Default cursor location

    /* Display Info Message */
    k_clear_screen();
    k_printf("\xB0\xB1\xB2\xDB Welcome to Choacury! \xDB\xB2\xB1\xB0", 0, 9);
    k_printf("Version: Build Feb 23rd 2024 (Pre-Terminal Shell)\n"                // <-- If the source code gets updated (even if it's not for the kernel), replace the date ;-)      
             "(C)opyright: \2 Pineconium 2023, 2024.", 1, 7);

    pic_init();     // <-- Enable clock stuff

    pit_init();     // <-- Enable Timer

    /* Enable interrupts to make keyboard work */
    asm volatile("sti");

    ps2_init();
    /* FIXME: support more keymaps :) */
    ps2_init_keymap_fi();

    StartUp_Beeps();
    
    /* Text Cursor control */
    // Note QEMU might cause some weird artifacts when loaded.
    port_byte_out(0x3d4, 14);
    int position = port_byte_in(0x3d5);
    position = position << 8;

    port_byte_out(0x3d4, 15);
    position += port_byte_in(0x3d5);

    /* Print PCI devices */
    debug_print_pci();

    /* Quick hack to print keyboard input */
    u16* vga_mem = (u16*)0xb8000;
    vga_mem += 80 * 4;

    /* Allow Keyboard Entry stuff */
    for (;;) {
        key_event_t event;
        ps2_get_key_event(&event);
        if (event.key == KEY_NONE || (event.modifiers & KEY_EVENT_MODIFIERS_RELEASED)) {
            asm volatile("hlt");
            continue;
        }

        /* Backspace handler */
        if (event.key == KEY_Backspace) {
            if (vga_mem > (u16*)0xb8000 + 80 * 4) {
                *(--vga_mem) = (TC_WHITE << 8) | ' ';   // <-- Replaces the previous character with a blank space.
            }
            prevX = prevX - 1;
            update_cursor(prevX, prevY);
            continue;
        }

        /* Enter Key/New Line Character */
        if (event.key == KEY_Enter) {
            u32 line = (vga_mem - (u16*)0xb8000) / 80;
            vga_mem = (u16*)0xb8000 + (line + 1) * 80;  // <-- Prints a new line, but it kinda fills the remaining spaces in the line above with space characters
                                                        // Meaning it can be annoying to edit the line above without arrow support.

            /* I assume we can just put many of the terminal stuff here. */

            prevY = prevY + 1;
            prevX = 0;
            update_cursor(prevX, prevY);
            continue;
        }

        /* Terminal Bell */
        if (event.key == KEY_LeftCtrl) {
            startbeep(800);
            pit_sleep_ms(15);
            mutebeep();
        }

        const char* utf8 = key_to_utf8(&event);
        if (!utf8)
            continue;

        while (*utf8) {
            *vga_mem++ = (TC_WHITE << 8) | *utf8++;
            prevX = prevX + 1;
            update_cursor(prevX, prevY);

        }
    }
};
