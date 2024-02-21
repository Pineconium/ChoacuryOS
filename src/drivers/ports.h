#ifndef PORTS_H
#define PORTS_H

#include "types.h"

u8 port_byte_in (u16 port);
void port_byte_out (u16 port, u8 data);
u16 port_word_in (u16 port);
void port_dword_out (u16 port, u32 data);
u32 port_dword_in (u16 port);

#endif