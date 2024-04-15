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
#include "../drivers/types.h"
#include "../drivers/vga.h"
#include "../drivers/pci.h"
#include "../shell/terminal.h"
#include "../shell/string_mang.h"               // <-- Cool little string system...

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
    startbeep(775);
    pit_sleep_ms(50);
    mutebeep();
}

/* This is how commands work */
void userinput(char* buffer){
  if(strcmp(buffer, "hello")==0){
    // if prints, terminal commands work!
    term_write("Hello from Terminal\n", TC_WHITE);
  }
  if(strcmp(buffer, "cls")==0){
    term_clear();
  }
}

/* A Simple kernel written in C */
void k_main() 
{
    gdt_init();
    idt_init();

    /* Display Info Message */
    term_init(VGA_WIDTH, VGA_HEIGHT, vga_set_char, vga_move_cursor);
    term_write("\xB0\xB1\xB2\xDB Welcome to Choacury! \xDB\xB2\xB1\xB0\n", TC_LIME);
    term_write("Version: Build " __DATE__ " (Terminal Testing)\n", TC_WHITE);
    term_write("(C)opyright: \2 Pineconium 2023, 2024.\n\n", TC_WHITE);

    pic_init();     // <-- Enable clock stuff

    pit_init();     // <-- Enable Timer

    /* Enable interrupts to make keyboard work */
    asm volatile("sti");

    ps2_init();
    /* FIXME: support more keymaps :) */
    ps2_init_keymap_us();

    StartUp_Beeps();

    /* Print PCI devices */
    debug_print_pci();

    char cmdbuffer[32];

    for (;;) {
        key_event_t event;
        ps2_get_key_event(&event);
        if (event.key == KEY_NONE) {
            asm volatile("hlt");
            continue;
        }
        if (event.modifiers & KEY_EVENT_MODIFIERS_RELEASED) {
            continue;
        }

        switch (event.key) {
            case KEY_Backspace:
                term_write("\b \b", TC_WHITE);
                backspace(cmdbuffer);    // <-- Removes last char from the buffer.
                break;
            case KEY_Enter:
                term_putchar('\n', TC_WHITE);
                userinput(cmdbuffer);
                cmdbuffer[0] = '\0';
                break;
            case KEY_LeftCtrl:
                startbeep(800);
                pit_sleep_ms(15);
                mutebeep();
                break;
            default: {
                const char* utf8 = key_to_utf8(&event);
                while (utf8 && *utf8) {
                    term_putchar(*utf8, TC_WHITE);
                    append(cmdbuffer, *utf8);
                    utf8++;
                }
                break;
            }
        }
    }
};
