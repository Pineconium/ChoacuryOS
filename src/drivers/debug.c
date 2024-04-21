#include "debug.h"
#include "ports.h"

void dprintchar(char c)
{
    port_byte_out(0x3F8, c);
}

void dprint(const char* message)
{
    for (int i = 0; message[i]; i++) {
        dprintchar(message[i]);
    }
}

void dprintln(const char* message)
{
    dprint(message);
    dprint("\r\n");
}

void dprintbyte(u8 byte) {
    dprintchar(((byte >>   4) < 10) ? (byte >>   4) + '0' : (byte >>   4) - 10 + 'a');
    dprintchar(((byte & 0x0F) < 10) ? (byte & 0x0F) + '0' : (byte & 0x0F) - 10 + 'a');
}

void dprintint(s64 value) {
    if (value < 0) {
        dprint("-");
        value = -value;
    }

    if (value == 0) {
        dprint("0");
        return;
    }

    char buffer[21];
    int len = 0;
    while (value > 0) {
        buffer[len++] = (value % 10) + '0';
        value /= 10;
    }

    for (int i = len - 1; i >= 0; i--) {
        dprintchar(buffer[i]);
    }
}
