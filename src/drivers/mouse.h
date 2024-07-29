/* actually moose drivers yay */

#pragma once

#include "types.h"
#include "pointerctrl.h"    // <-- where 'Point' is

extern Point MousePosition;
extern Point MousePositionOld;

#define PS2Leftbutton 0b00000001
#define PS2Middlebutton 0b00000100
#define PS2Rightbutton 0b00000010
#define PS2XSign 0b00010000
#define PS2YSign 0b00100000
#define PS2XOverflow 0b01000000
#define PS2YOverflow 0b10000000

u8 MousePacket[4];
int PacketReady = 0;
u8 Cycle = 0;

void mouse_init();
void mouse_write(uint8_t value);
uint8_t mouse_read();
void mouse_wait();
void mouse_wait_input();
void handle_mouse(uint8_t data);
void process_mouse_pack();
