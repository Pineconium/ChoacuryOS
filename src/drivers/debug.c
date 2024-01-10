#include "debug.h"
#include "ports.h"

void dprint(const char* message)
{
    for (int i = 0; message[i]; i++) {
        port_byte_out(0x3F8, message[i]);
    }
}

void dprintln(const char* message)
{
    dprint(message);
    dprint("\r\n");
}

void dprintbyte(u8 byte) {
    char buffer[3];
    buffer[0] = ((byte >>   4) < 10) ? (byte >>   4) + '0' : (byte >>   4) - 10 + 'a';
    buffer[1] = ((byte & 0x0F) < 10) ? (byte & 0x0F) + '0' : (byte & 0x0F) - 10 + 'a';
    buffer[2] = '\0';
    dprint(buffer);
}
