/* actually moose drivers yay */

#pragma once

#include "types.h"

void mouse_init();
void mouse_write(uint8_t value);
uint8_t mouse_read();
void mouse_wait();
void mouse_wait_input();
