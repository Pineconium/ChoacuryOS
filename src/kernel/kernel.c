/*

    :'######::'##::::'##::'#######:::::'###::::'##:::'##:'########:'########::'##::: ##:
    '##... ##: ##:::: ##:'##.... ##:::'## ##::: ##::'##:: ##.....:: ##.... ##: ###:: ##:
     ##:::..:: ##:::: ##: ##:::: ##::'##:. ##:: ##:'##::: ##::::::: ##:::: ##: ####: ##:
     ##::::::: #########: ##:::: ##:'##:::. ##: #####:::: ######::: ########:: ## ## ##:
     ##::::::: ##.... ##: ##:::: ##: #########: ##. ##::: ##...:::: ##.. ##::: ##. ####:
     ##::: ##: ##:::: ##: ##:::: ##: ##.... ##: ##:. ##:: ##::::::: ##::. ##:: ##:. ###:
    . ######:: ##:::: ##:. #######:: ##:::: ##: ##::. ##: ########: ##:::. ##: ##::. ##:
    :......:::..:::::..:::.......:::..:::::..::..::::..::........::..:::::..::..::::..::
    The Choacury Kernel.
    The code still needs to be finished!

    (C) Pineconium Software, Made in 2023.
    Revision - Aug 25th 2023
*/



#include "../drivers/ports.h"   // The '..' are used because the compiler doesn't EXACTLY know where it is :/
#include "../drivers/idt.h"
#include "../drivers/types.h"

/* The Kernel itself */
void k_main() 
{
    k_clear_screen();
    k_printf("WELCOME TO CHOAKERN. The Choacury Kernel!\nVersion: Applecake 2 Internal\nCopyright (C) Pineconium 2023-\n\nGraphics Type : Standard Text Mode (soon gonna be VESA)\nKeyboard input: Under construction...", 0);
    port_byte_out(0x3d4, 14);
    int position = port_byte_in(0x3d5);
    position = position << 8;

    port_byte_out(0x3d4, 15);
    position += port_byte_in(0x3d5);

    int offset_from_vga = position * 2;
};