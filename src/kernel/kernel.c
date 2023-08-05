/*
The kernel of Choacury
I call it the Choakernel! The code still needs to be finished 
*/
#include "../drivers/ports.h"
#include "../drivers/idt.h"
#include "../drivers/types.h"

/* simple kernel written in C */
void k_main() 
{
    k_clear_screen();
    k_printf("WELCOME TO CHOAKERN. The Choacury Kernel!\nVersion: Internal Pharaoh 1\nCopyright (C) Pineconium 2023-\n\nUNDERCONSTRUCTION", 0);
    port_byte_out(0x3d4, 14);
    int position = port_byte_in(0x3d5);
    position = position << 8;

    port_byte_out(0x3d4, 15);
    position += port_byte_in(0x3d5);

    int offset_from_vga = position * 2;
};