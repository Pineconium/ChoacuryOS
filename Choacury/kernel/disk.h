#ifndef __DISK_H
#define __DISK_H

#include <stddef.h>
#include <stdlib.h>

#include   "GIC.h"
#include "PL011.h"
#include "SP804.h"

#define RETRY ( 3 )

// query the disk block count
extern uint32_t disk_get_block_num();
// query the disk block length
extern uint32_t disk_get_block_len();

// write an n-byte block of data x to   the disk at block address a
extern void     disk_wr( uint32_t a, const uint8_t* x, int n );
// read  an n-byte block of data x from the disk at block address a
extern void     disk_rd( uint32_t a,       uint8_t* x, int n );

#endif
