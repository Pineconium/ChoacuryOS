#include "timer.h"
#include "vga.h"
#include "utils.h"
#include "isr.h"

u32 tick = 0;

static void timer_callback(registers_t regs) {
    tick++;
    k_printf("Tick: ", 0);
    
    char tick_ascii[256];
    // int_to_ascii(tick, tick_ascii);
    k_printf(tick_ascii, 0);
    k_printf("\n", 0);
}

void init_timer(u32 freq) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    u32 divisor = 1193180 / freq;
    u8 low  = (u8)(divisor & 0xFF);
    u8 high = (u8)( (divisor >> 8) & 0xFF);
    /* Send the command */
    port_byte_out(0x43, 0x36); /* Command port */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}