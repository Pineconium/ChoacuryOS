/*
    The kernel of Choacury
    I call it the Choakern! The code still needs to be finished tho...
*/

/* Includes needed for the kernel to actually work.*/
#include "../drivers/gdt.h"
#include "../drivers/idt.h"
#include "../drivers/keymaps/ps2_keymap_us.h"   // <-- US Keyboard Layout.
#include "../drivers/mouse.h"
#include "../drivers/pci.h"
#include "../drivers/pic.h"
#include "../drivers/pit.h"
#include "../drivers/ports.h"
#include "../drivers/ps2.h"
#include "../drivers/sound.h"
#include "../drivers/storage/device.h"
#include "../drivers/types.h"
#include "../drivers/utils.h"
#include "../drivers/vga.h"
//#include "../drivers/fat.h"
#include "../memory/kmalloc.h"
#include "../memory/pmm.h"
#include "../shell/shell.h"
#include "../shell/terminal.h"
#include "multiboot.h"
#include "panic.h"
#include "process.h"

extern void (*__init_array[])();
extern void (*__init_array_end[])();

/* Startup Beep*/
void StartUp_Beeps() {
    startbeep(450);
    pit_sleep_ms(100);
    mutebeep();
    startbeep(775);
    pit_sleep_ms(50);
    mutebeep();
}

/* A Simple kernel written in C
 * These parameters are pushed onto the stack by the assembly kernel entry file.
 */

void k_main(multiboot_info_t* mbd, uint32_t magic) {
    gdt_init();
    idt_init();
    kmalloc_init();

    vga_text_init(TC_BLACK);

    u32 term_width = VGA_width;               // Set this to the width of your VGA text mode
    u32 visible_height = VGA_height;          // Set this to the height of your VGA text mode (visible height)
    u32 buffer_height = visible_height + 100; // Set this to a height larger than visible to allow scrolling

    term_init(term_width, buffer_height, visible_height, vga_set_char, vga_move_cursor);

    term_write("\n\xB0\xB1\xB2\xDB Welcome to Choacury! \xDB\xB2\xB1\xB0\n", TC_LIME);
    term_write("Version: Build " __DATE__ " (GUI Testing)\n", TC_WHITE);
    term_write("(C)opyright: \2 Pineconium 2023-2025.\n\n", TC_WHITE);

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("Bootloader did not provide multiboot information\n");
    }

    // For the stuff on lines 30-31
    for (size_t i = 0; &__init_array[i] != __init_array_end; i++)
    {
        __init_array[i]();
    }

    pmm_init(mbd);
    pic_init();
    pit_init();

    asm volatile("sti");

    ps2_init();
    ps2_init_keymap_us();

    // StartUp_Beeps();

    storage_device_init();
    debug_print_pci();
	

    shell_start();
}
