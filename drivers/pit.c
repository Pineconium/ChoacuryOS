#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "ports.h"

#define PIT_IRQ 0

#define TIMER0_CTL			0x40
#define PIT_CTL				0x43

#define SELECT_CHANNEL0		0x00

#define ACCESS_HI 			0x10
#define ACCESS_LO 			0x20

#define MODE_SQUARE_WAVE	0x06

#define BASE_FREQUENCY		1193182

static volatile u64 s_system_time = 0;

static void pit_irq_handler() {
    s_system_time++;
}

void pit_init() {
    port_byte_out(PIT_CTL, SELECT_CHANNEL0 | ACCESS_LO | ACCESS_HI | MODE_SQUARE_WAVE);

    // tick every ms
    const u16 timer_reload = BASE_FREQUENCY / 1000;
    port_byte_out(TIMER0_CTL, (timer_reload >> 0) & 0xff);
    port_byte_out(TIMER0_CTL, (timer_reload >> 8) & 0xff);

    s_system_time = 0;

    idt_register_irq_handler(PIT_IRQ, pit_irq_handler);
    pic_unmask(PIT_IRQ);
}

void pit_sleep_ms(u64 ms) {
    u64 wake_time = s_system_time + ms;
    while (s_system_time < wake_time)
        continue;
}

u64 pit_current_time_ms() {
    return s_system_time;
}
