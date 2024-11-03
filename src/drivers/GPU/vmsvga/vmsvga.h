/*
	VMWARE SVGA II DRIVER HEADER
	(everything is in here btw)
*/
#pragma once
#include "../../types.h"
#define VMWARE_VENDOR_ID 0x15AD
#define VMWARE_DEVICE_ID 0x0405


#define SVGA_INDEX_PORT_OFFSET 0
#define SVGA_VALUE_PORT_OFFSET 1
#define SVGA_PORT_BASE (bar0 - 1)

// Port offsets from the base I/O port
#define SVGA_INDEX_PORT_OFFSET 0   // SVGA_INDEX register offset
#define SVGA_VALUE_PORT_OFFSET 1   // SVGA_VALUE register offset
#define SVGA_BIOS_PORT_OFFSET 2    // SVGA_BIOS register offset
#define SVGA_IRQSTATUS_PORT_OFFSET 8 // SVGA_IRQSTATUS register offset

// Register indices within the SVGA device
#define SVGA_REG_ID 0               // Negotiation specification ID
#define SVGA_REG_ENABLE 1           // Enable SVGA mode
#define SVGA_REG_WIDTH 2            // Current screen width
#define SVGA_REG_HEIGHT 3           // Current screen height
#define SVGA_REG_MAX_WIDTH 4        // Max supported screen width
#define SVGA_REG_MAX_HEIGHT 5       // Max supported screen height
#define SVGA_REG_BPP 7              // Screen bits per pixel
#define SVGA_REG_FB_START 13        // Frame buffer start address
#define SVGA_REG_FB_OFFSET 14       // Frame buffer offset to visible pixels
#define SVGA_REG_VRAM_SIZE 15       // Video RAM size
#define SVGA_REG_FB_SIZE 16         // Frame buffer size
#define SVGA_REG_CAPABILITIES 17    // Device capabilities
#define SVGA_REG_FIFO_START 18      // FIFO start address
#define SVGA_REG_FIFO_SIZE 19       // FIFO size
#define SVGA_REG_CONFIG_DONE 20     // FIFO operation enable flag
#define SVGA_REG_SYNC 21            // Flush FIFO changes
#define SVGA_REG_BUSY 22            // FIFO processing flag

// FIFO register indices
#define SVGA_FIFO_MIN 0             // Start of the command queue
#define SVGA_FIFO_MAX 1             // End of the command queue
#define SVGA_FIFO_NEXT_CMD 2        // Next command offset
#define SVGA_FIFO_STOP 3            // Stop command

// Commands
#define SVGA_CMD_UPDATE 1           // Screen update command
#define SVGA_CMD_RECT_COPY 3        // Rectangle copy command
#define SVGA_CMD_DEFINE_CURSOR 19   // Define cursor command
#define SVGA_CMD_DEFINE_ALPHA_CURSOR 22 // Define alpha cursor


void init_vmware_svga(u32 base_port);
void init_fifo(u32 base_port, u32 fifo_start, u32 fifo_size);
u32 get_svga_base_port(u16 bus, u16 device, u16 function);
int detect_vmware_svga();
u32 read_register(u32 base_port, u32 reg);
void write_register(u32 base_port, u32 reg, u32 value);
void fifo_write_cmd(u32 base_port, u32 cmd, u32* operands, u32 num_operands);
void write_fifo(u32 base_port, u32 value);
void set_display_mode(u32 base_port, u32 width, u32 height, u32 bpp);
void increment_fifo_next_cmd(u32 base_port);
void fifo_cmd_update(u32 base_port, u32 fifo_start, u32 start_x, u32 start_y, u32 width, u32 height);
