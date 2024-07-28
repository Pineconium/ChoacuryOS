/* actually moose drivers yay */

#pragma once

#include "ports.h"
#include "types.h"
#include "pic.h"

/* well I just need actual code */

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

void mouse_write(){
    return; // <-- Replace
}

void mouse_read(){
    return; // <-- Replace
}

void mouse_wait(){
    return; // <-- Replace
}

void mouse_wait_input(){
    return; // <-- Replace
}

/* grab the cursors position, meant for a cursor */
void mouse_pos(){
    return; // <-- Replace
}
