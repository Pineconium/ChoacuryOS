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
#include "../drivers/keymaps/ps2_keymap_us.h"   // <-- US Keyboard Layout.
#include "../drivers/sound.h"
#include "../drivers/storage/device.h"
#include "../drivers/types.h"
#include "../drivers/vga.h"
#include "../drivers/pci.h"
#include "../shell/shell.h"
#include "../shell/terminal.h"
#include <memory/kmalloc.h>

/* Startup Beep*/
void StartUp_Beeps() {
    startbeep(450);
    pit_sleep_ms(100);
    mutebeep();
    startbeep(775);
    pit_sleep_ms(50);
    mutebeep();
}

/* A Simple kernel written in C */
void k_main()
{
    gdt_init();
    idt_init();
    kmalloc_init();

    /* Display Info Message */
    term_init(VGA_WIDTH, VGA_HEIGHT, vga_set_char, vga_move_cursor);
    term_write("\xB0\xB1\xB2\xDB Welcome to Choacury! \xDB\xB2\xB1\xB0\n", TC_LIME);
    term_write("Version: Build " __DATE__ " (FS Testing)\n", TC_WHITE);
    term_write("(C)opyright: \2 Pineconium 2023, 2024.\n\n", TC_WHITE);

    pic_init();     // <-- Enable clock stuff

    pit_init();     // <-- Enable Timer

    /* Enable interrupts to make keyboard work */
    asm volatile("sti");

    ps2_init();
    /* FIXME: support more keymaps :) */
    ps2_init_keymap_us();

    StartUp_Beeps();

    /* Initialize storage devices */
    storage_device_init();

    /* Print PCI devices */
    debug_print_pci();

    shell_start();
};
