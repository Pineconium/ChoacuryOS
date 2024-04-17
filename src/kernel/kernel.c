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
#include "../drivers/types.h"
#include "../drivers/vga.h"
#include "../drivers/pci.h"
#include "../shell/terminal.h"
#include "../shell/string_mang.h"               // <-- Cool little string system...

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
    // TODO: Add arguments into commands (e.g. "echo [STRING]")
  if(strcmp(buffer, "hello")==0){
    // Standard Testing commands
    term_write("Hello from Terminal\n", TC_WHITE);
  }
  if(strcmp(buffer, "help")==0){
    term_write("LIST OF COMMANDS\n", TC_WHITE);
    term_write("help            - Hello there! I'm the Help Command!\n", TC_WHITE);
    term_write("compdate        - Shows the compilation date.\n", TC_WHITE);
    term_write("cls             - Clears the screen.\n", TC_WHITE);
  }
  if(strcmp(buffer, "cls")==0){
    term_clear();
  }
  if(strcmp(buffer, "compdate")==0){
    term_write(__DATE__ "\n", TC_WHITE);
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
