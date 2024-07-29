/* actually moose drivers yay */

#include "ports.h"
#include "types.h"
#include "pic.h"
#include "mouse.h"

void mouse_init(){
    port_byte_out(0x64, 0xA8);

    mouse_wait();
    port_byte_out(0x64, 0x20);
    mouse_wait_input();
    uint8_t status = port_byte_in(0x60);
    status |= 0b10;
    mouse_wait();
    port_byte_out(0x64, 0x60);
    mouse_wait();
    port_byte_out(0x60, status);

    mouse_write(0xF6);
    mouse_read();

    mouse_write(0xF4);
    mouse_read();
}

void mouse_write(uint8_t value){
    mouse_wait();
    port_byte_out(0x64, 0xD4);
    mouse_wait();
    port_byte_out(0x60, value);
}

uint8_t mouse_read(){
    mouse_wait_input();
    return port_byte_in(0x60);
}

void mouse_wait(){
    uint64_t timeout = 100000;
    while (timeout--){
        if ((port_byte_in(0x64) & 0b10) == 0){
            return;
        }
    }
}

void mouse_wait_input(){
    uint64_t timeout = 100000;
    while (timeout--){
        if (port_byte_in(0x64) & 0b1){
            return;
        }
    }
}

/* the mouse cursor */
uint8_t Cursor[] = {
    // 16x2
    0b00100000, 0b00000000,
    0b00110000, 0b00000000,
    0b00101000, 0b00000000,
    0b00100100, 0b00000000,
    0b00100010, 0b00000000,
    0b00100001, 0b00000000,
    0b00100000, 0b10000000,
    0b00100000, 0b01000000,
    0b00100000, 0b00100000,
    0b00100000, 0b00010000,
    0b00100000, 0b01111000,
    0b00100110, 0b01000000,
    0b00101001, 0b00100000,
    0b00010000, 0b10010000,
    0b00000000, 0b01001000,
    0b00000000, 0b00110000
};