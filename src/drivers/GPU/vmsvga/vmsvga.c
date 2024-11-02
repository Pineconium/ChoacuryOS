#include "vmsvga.h"
#include "../../pci.h"
#include "../../types.h"
#include "../../debug.h"

static inline void outportl(unsigned short port, unsigned int data) {
    __asm__ volatile ("outl %0, %1" : : "a"(data), "Nd"(port));
}

static inline unsigned int inportl(unsigned short port) {
    unsigned int result;
    __asm__ volatile ("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}


/* Vmware svga II utility functions*/
void write_register(u32 base_port, u32 reg, u32 value) {
    outportl(base_port + SVGA_INDEX_PORT_OFFSET, reg);
    outportl(base_port + SVGA_VALUE_PORT_OFFSET, value);
}

u32 read_register(u32 base_port, u32 reg) {
    outportl(base_port + SVGA_INDEX_PORT_OFFSET, reg);
    return inportl(base_port + SVGA_VALUE_PORT_OFFSET);
}

void write_fifo(u32 base_port, u32 value) {
    u32 fifo_offset = read_register(base_port, SVGA_FIFO_NEXT_CMD);
    outportl(base_port + fifo_offset, value);
    write_register(base_port, SVGA_FIFO_NEXT_CMD, fifo_offset + 4);
}


int detect_vmware_svga() {
    for (int bus = 0; bus < 8; ++bus) {
        for (int device = 0; device < 32; ++device) {
            for (int function = 0; function < 8; ++function) {
                u32 vendor_device_id = read_pci(bus, device, function, 0x00);
                if ((vendor_device_id & 0xFFFF) == VMWARE_VENDOR_ID &&
                    (vendor_device_id >> 16) == VMWARE_DEVICE_ID) {
                    dprintln("VMWARE SVGA FOUND");
					return 1;  // Device found
                }
            }
        }
    }
	dprintln("VMWARE SVGA NOT FOUND");
    return 0;  // Device not found
	
}

u32 get_svga_base_port(uint16_t bus, uint16_t device, uint16_t function) {
    u32 bar0 = read_pci(bus, device, function, 0x10);  // BAR0
    return (bar0 & ~0x3) - 1;
}

void fifo_write_cmd(u32 base_port, u32 cmd, u32* operands, u32 num_operands) {
    write_fifo(base_port, cmd);
    for (u32 i = 0; i < num_operands; ++i) {
        write_fifo(base_port, operands[i]);
    }
}

void init_fifo(u32 base_port, u32 fifo_start, u32 fifo_size) {
    // Set FIFO minimum command offset (start of the command queue)
    write_register(base_port, SVGA_FIFO_MIN, 293 * 4);  // 293 32-bit registers

    // Set FIFO maximum command offset (end of the queue)
    write_register(base_port, SVGA_FIFO_MAX, fifo_size);

    // Set the next command offset (initially the start of the command queue)
    write_register(base_port, SVGA_FIFO_NEXT_CMD, 293 * 4);

    // Set the stop command offset to the end of the FIFO size
    write_register(base_port, SVGA_FIFO_STOP, fifo_size);

    // Mark the FIFO as fully configured by writing 1 to SVGA_REG_CONFIG_DONE
    write_register(base_port, SVGA_REG_CONFIG_DONE, 1);
}

void set_display_mode(u32 base_port, u32 width, u32 height, u32 bpp) {
    // Set the desired screen width, height, and bits per pixel
    write_register(base_port, SVGA_REG_WIDTH, width);
    write_register(base_port, SVGA_REG_HEIGHT, height);
    write_register(base_port, SVGA_REG_BPP, bpp);

    // Enable SVGA mode to apply changes
    write_register(base_port, SVGA_REG_ENABLE, 1);
}

void fifo_cmd_update(u32 base_port, u32 fifo_start, u32 start_x, u32 start_y, u32 width, u32 height) {
    // Write the update command ID to FIFO
    *((volatile u32 *)(fifo_start + read_register(base_port, SVGA_FIFO_NEXT_CMD))) = SVGA_CMD_UPDATE;
    increment_fifo_next_cmd(base_port);

    // Write the parameters for the update
    *((volatile u32 *)(fifo_start + read_register(base_port, SVGA_FIFO_NEXT_CMD))) = start_x;
    increment_fifo_next_cmd(base_port);
    *((volatile u32 *)(fifo_start + read_register(base_port, SVGA_FIFO_NEXT_CMD))) = start_y;
    increment_fifo_next_cmd(base_port);
    *((volatile u32 *)(fifo_start + read_register(base_port, SVGA_FIFO_NEXT_CMD))) = width;
    increment_fifo_next_cmd(base_port);
    *((volatile u32 *)(fifo_start + read_register(base_port, SVGA_FIFO_NEXT_CMD))) = height;
    increment_fifo_next_cmd(base_port);

    // Trigger the FIFO to process the commands by setting the SYNC register
    write_register(base_port, SVGA_REG_SYNC, 1);

    // Wait until the SVGA device completes processing the commands
    while (read_register(base_port, SVGA_REG_BUSY)) {
        // Poll until not busy
    }
}

void increment_fifo_next_cmd(u32 base_port) {
    // Increment the SVGA_FIFO_NEXT_CMD by 4 bytes (size of each entry)
    u32 next_cmd = read_register(base_port, SVGA_FIFO_NEXT_CMD);
    write_register(base_port, SVGA_FIFO_NEXT_CMD, next_cmd + 4);
}


void init_vmware_svga(u32 base_port) {
    // Set the specification ID
    outportl(base_port + SVGA_INDEX_PORT_OFFSET, 0);
    outportl(base_port + SVGA_VALUE_PORT_OFFSET, 0x90000002);
    
    // Verify if device is compatible
    u32 id = inportl(base_port + SVGA_VALUE_PORT_OFFSET);
    if (id != 0x90000002) {
		dprintln("DEVICE NOT SUPPORTED!!!");
        return;  // Device not supported
    }
    
    // Read frame buffer address and FIFO settings
    u32 fb_start = read_register(base_port, SVGA_REG_FB_START);
    u32 fifo_start = read_register(base_port, SVGA_REG_FIFO_START);
    u32 fifo_size = read_register(base_port, SVGA_REG_FIFO_SIZE);
    
    // Initialize FIFO settings
    init_fifo(base_port, fifo_start, fifo_size);

    // Enable the SVGA mode
    write_register(base_port, SVGA_REG_ENABLE, 1);
	// -- This wont work, it will be "initialized" but it wants the FIFO memory to be identity mapped and we dont have paging --
}
