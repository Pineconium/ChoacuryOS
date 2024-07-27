/*
    The kernel of Choacury
    I call it the Choakern! The code still needs to be finished tho...
*/

/* Includes needed for the kernel to actually work.*/
#include <stdint.h>
#include "../drivers/ports.h"
#include "panic.h"
#include "../drivers/gdt.h"
#include "multiboot.h"
#include "../drivers/idt.h"
#include "../drivers/pic.h"
#include "../drivers/utils.h"
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

/* Gets the complete size of the memory */
uint64_t detect_memory(multiboot_info_t* mbd, uint32_t magic) {
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
        panic_impl("KERNEL:", "Invalid memory map (Invalid magic number)\n");
    if (!(mbd->flags >> 6 & 0x1))
        panic_impl("KERNEL:", "Invalid memory map (Wrong flags)\n");
    int num_memory_blocks = mbd->mmap_length - 1;
    multiboot_memory_map_t* largest_block = 
            (multiboot_memory_map_t*) (mbd->mmap_addr + num_memory_blocks - sizeof(multiboot_memory_map_t));
    uint64_t memory_size_bytes = largest_block->addr + largest_block->len;
    /* Convert it to megabytes
     * Seems like a simple division? Think again.
     * In this freestanding environment, there's no access to division of unsigned integers.
     * Therefore, I wrote a simple divider just for this.
     * Idk if it's the best implementation, but it should work.
     * It simply uses repeated subtraction.
     */
    int memory_size_mb = 0;
    while (memory_size_bytes >= 1000000) {
        memory_size_bytes -= 1000000;
        memory_size_mb++;
    }
    return memory_size_mb;
} 

/* A Simple kernel written in C 
 * These parameters are pushed onto the stack by the assembly kernel entry file.
 */
void k_main(multiboot_info_t* mbd, uint32_t magic) {
    gdt_init();
    idt_init();
    kmalloc_init();
    /* Display Info Message */
    term_init(VGA_WIDTH, VGA_HEIGHT, vga_set_char, vga_move_cursor); 
    term_write("\n\xB0\xB1\xB2\xDB Welcome to Choacury! \xDB\xB2\xB1\xB0\n", TC_LIME);
    term_write("Version: Build " __DATE__ " (FS Testing)\n", TC_WHITE);
    term_write("(C)opyright: \2 Pineconium 2023, 2024.\n\n", TC_WHITE);
     
    uint64_t memory = detect_memory(mbd, magic);    // <-- Used in the chstat command
    
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

    shell_start(memory);
};
