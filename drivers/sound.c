#include "ports.h"
#include "sound.h"

/* PC Speaker Stuff */
void startbeep(u32 nFrequence) {
    /* Sets the PIT to the desired freq */
    u32 div = 1193180 / nFrequence;
    port_byte_out(0x43, 0xb6);
    port_byte_out(0x42, (u8) (div));
    port_byte_out(0x42, (u8) (div >> 8));

    /* Now lets actually BEEP */
    u8 tmp = port_byte_in(0x61);
    if (tmp != (tmp | 3)) {
        port_byte_out(0x61, tmp | 3);
    }
}

/* Make the beeper shut up */
void mutebeep() {
    u8 tmp = port_byte_in(0x61) & 0xFC;
    port_byte_out(0x61, tmp);
}
