/*
    The kernel of Choacury
    I call it the Choakern! The code still needs to be finished tho...
*/

/* Includes needed for the kernel to actually work.*/
#include "../drivers/ports.h"
#include "../drivers/gdt.h"
#include "../drivers/idt.h"
#include "../drivers/pic.h"
#include "../drivers/ps2.h"
#include "../drivers/ps2_keymap_fi.h"
#include "../drivers/types.h"
#include "../drivers/vga.h"

/* A Simple kernel written in C */
void k_main() 
{
    gdt_init();
    idt_init();

    /* Display Info Message */
    k_clear_screen();
    k_printf("Welcome to Choacury!", 0, 6);
    k_printf("Version: Post Reset Build Jan 2nd 2024\n"                
             "Copyright (C) \2 Pineconium 2023-\n", 1, 15);
    
    pic_init();

    ps2_init();
    /* FIXME: support more keymaps :) */
    ps2_init_keymap_fi();
    
    /* DOS 16 Colour Test */
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 6, 4); 
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 7, 6); 
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 8, 2);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 9, 3);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 10, 1);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 11, 5);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 12, 7);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 13, 8);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 6, 12); 
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 7, 14); 
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 8, 10);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 9, 11);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 10, 9);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 11, 13);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 12, 15);

    /* I think we should put some keyboard control here */

    /* Text Cursor control */
    // Note QEMU might cause some weird artifacts when loaded.
    port_byte_out(0x3d4, 14);
    int position = port_byte_in(0x3d5);
    position = position << 8;

    port_byte_out(0x3d4, 15);
    position += port_byte_in(0x3d5);

    int offset_from_vga = position * 2;

    /* Enable interrupts to make keyboard work */
    asm volatile("sti");

    /* Quick hack to print keyboard input */
    u16* vga_mem = (u16*)0xb8000;
    vga_mem += 80 * 4;

    for (;;) {
        key_event_t event;
        ps2_get_key_event(&event);

        if (event.key == KEY_NONE || (event.modifiers & KEY_EVENT_MODIFIERS_RELEASED)) {
            asm volatile("hlt");
            continue;
        }

        const char* utf8 = key_to_utf8(&event);
        if (!utf8)
            continue;

        while (*utf8) {
            *vga_mem++ = (TC_BRIGHT << 8) | *utf8++;
        }
    }
};
