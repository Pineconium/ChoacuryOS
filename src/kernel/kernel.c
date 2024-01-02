/*
    The kernel of Choacury
    I call it the Choakern! The code still needs to be finished tho...
*/

/* Includes needed for the kernel to actually work.*/
#include "../drivers/ports.h"
#include "../drivers/gdt.h"
#include "../drivers/idt.h"
#include "../drivers/pic.h"
#include "../drivers/types.h"
#include "../drivers/vga.h"

/* A Simple kernel written in C */
void k_main() 
{
    gdt_init();
    idt_init();

    /* Display Info Message */
    k_clear_screen();
    k_printf("WELCOME TO CHOAKERN\nThe Choacury Kernel!\n"
             "Version: Post Reset Build Dec 25th 2023\n"                
             "Copyright (C) \2 Pineconium 2023-\n", 0, 7);
    
    pic_init();

    k_printf("No Keyboard Input :(", 5, 12);       // 6 months or so and still no keyboard input ._.
    
    /* DOS 16 Colour Test */
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 6, 4); 
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 7, 6); 
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 8, 2);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 9, 3);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 10, 1);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 11, 5);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 12, 7);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 13, 8);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 14, 12); 
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 15, 14); 
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 16, 10);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 17, 11);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 18, 9);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 19, 13);
    k_printf("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB", 20, 15);

    /* I think we should put some keyboard control here */

    /* Text Cursor control */
    // Note QEMU might cause some weird artifacts when loaded.
    port_byte_out(0x3d4, 14);
    int position = port_byte_in(0x3d5);
    position = position << 8;

    port_byte_out(0x3d4, 15);
    position += port_byte_in(0x3d5);

    int offset_from_vga = position * 2;
};
