#include "../debug.h"
#include "../ports.h"
#include "../utils.h"
#include "ata.h"
#include "device.h"
#include <memory/kmalloc.h>

#define ATA_IO_REG_DATA 0
#define ATA_IO_REG_ERROR 1
#define ATA_IO_REG_FEATURES 1
#define ATA_IO_REG_SECTOR_COUNT 2
#define ATA_IO_REG_LBA0 3
#define ATA_IO_REG_LBA1 4
#define ATA_IO_REG_LBA2 5
#define ATA_IO_REG_DEVICE_SELECT 6
#define ATA_IO_REG_COMMAND 7
#define ATA_IO_REG_STATUS 7

#define ATA_CTRL_REG_ALT_STATUS 0
#define ATA_CTRL_REG_DEVICE_CONTROL 0
#define ATA_CTRL_REG_DRIVE_ADDRESS 1

#define ATA_STATUS_ERR (1 << 0)
#define ATA_STATUS_IDX (1 << 1)
#define ATA_STATUS_CORR (1 << 4)
#define ATA_STATUS_DRQ (1 << 3)
#define ATA_STATUS_SRV (1 << 4)
#define ATA_STATUS_DF (1 << 5)
#define ATA_STATUS_RDY (1 << 6)
#define ATA_STATUS_BSY (1 << 7)

typedef struct {
    storage_device_t device;
    u16 io_base;
    u16 control_base;
    u8 index;
    char model[41];
} ata_device_t;

// 400 ns delay
static void ata_select_delay(u16 control_base) {
    for (int i = 0; i < 4; i++) {
        port_byte_in(control_base + ATA_CTRL_REG_ALT_STATUS);
    }
}

static bool ata_read_sectors(void* self, void* buffer, u64 sector, u64 count) {
    if (sector + count > (1 << 28)) {
        dprintln("ATA read: only 28-bit LBA is supported");
        return false;
    }
    if (count > 0xFF) {
        dprintln("ATA read: only 8-bit sector count is supported");
        return false;
    }

    ata_device_t* device = (ata_device_t*)self;

    if (sector + count > device->device.sector_count) {
        dprintln("ATA read: sector out of range");
        return false;
    }

    // select device
    port_byte_out(device->io_base + ATA_IO_REG_DEVICE_SELECT, 0xE0 | (device->index << 4) | ((sector >> 24) & 0x0F));
    ata_select_delay(device->control_base);

    // send read command
    port_byte_out(device->io_base + ATA_IO_REG_SECTOR_COUNT, count);
    port_byte_out(device->io_base + ATA_IO_REG_LBA0, sector >>  0);
    port_byte_out(device->io_base + ATA_IO_REG_LBA1, sector >>  8);
    port_byte_out(device->io_base + ATA_IO_REG_LBA2, sector >> 16);
    port_byte_out(device->io_base + ATA_IO_REG_COMMAND, 0x20);

    for (u64 i = 0; i < count; i++) {
        // wait for BSY to clear
        u8 status = port_byte_in(device->io_base + ATA_IO_REG_STATUS);
        while (status & ATA_STATUS_BSY) {
            status = port_byte_in(device->io_base + ATA_IO_REG_STATUS);
        }

        // wait for DRQ or ERR
        while (!(status & (ATA_STATUS_DRQ | ATA_STATUS_ERR))) {
            status = port_byte_in(device->io_base + ATA_IO_REG_STATUS);
        }

        // check for error
        if (status & ATA_STATUS_ERR) {
            dprintln("ATA read: error");
            return false;
        }

        // read data
        const u32 words_per_sector = device->device.sector_size / 2;
        for (u32 j = 0; j < words_per_sector; j++) {
            ((u16*)buffer)[i * words_per_sector + j] = port_word_in(device->io_base + ATA_IO_REG_DATA);
        }
    }

    return true;
}

static bool ata_write_sectors(void* self, const void* buffer, u64 sector, u64 count) {
    (void)self;
    (void)buffer;
    (void)sector;
    (void)count;
    dprintln("ATA write not implemented");
    return false;
}

static void ata_bus_init(u16 io_base, u16 control_base) {
    // check if there is no bus connected
    if (port_byte_in(io_base + ATA_IO_REG_STATUS) == 0xFF) {
        return;
    }

    for (int i = 0; i < 2; i++) {
        // select device
        port_byte_out(io_base + ATA_IO_REG_DEVICE_SELECT, 0xA0 | (i << 4));
        ata_select_delay(control_base);

        // send ATA identify command
        port_byte_out(io_base + ATA_IO_REG_LBA0, 0);
        port_byte_out(io_base + ATA_IO_REG_LBA1, 0);
        port_byte_out(io_base + ATA_IO_REG_LBA2, 0);
        port_byte_out(io_base + ATA_IO_REG_COMMAND, 0xEC);

        // check if device is present
        u8 status = port_byte_in(io_base + ATA_IO_REG_STATUS);
        if (status == 0) {
            continue;
        }

        // wait for BSY to clear
        while (status & ATA_STATUS_BSY) {
            status = port_byte_in(io_base + ATA_IO_REG_STATUS);
        }

        // check LBA1 and LBA2 to see if the device is not ATA
        if (port_byte_in(io_base + ATA_IO_REG_LBA1) != 0 || port_byte_in(io_base + ATA_IO_REG_LBA2) != 0) {
            continue;
        }

        // wait for DRQ or ERR
        while (!(status & (ATA_STATUS_DRQ | ATA_STATUS_ERR))) {
            status = port_byte_in(io_base + ATA_IO_REG_STATUS);
        }

        // check for error
        if (status & ATA_STATUS_ERR) {
            dprint("ATA device on bus 0x");
            dprintbyte(io_base >> 8);
            dprintbyte(io_base & 0xFF);
            dprint(" device ");
            dprintint(i);
            dprintln(" returned error");
            continue;
        }

        // read device info
        u16 data[256];
        for (int i = 0; i < 256; i++) {
            data[i] = port_word_in(io_base + ATA_IO_REG_DATA);
        }

        // get sector count
        u64 sector_count = 0;
        if (data[83] & (1 << 10)) {
            sector_count = data[100] | (data[101] << 16) | ((u64)data[102] << 32) | ((u64)data[103] << 48);
        } else {
            sector_count = data[60] | (data[61] << 16);
        }

        // get sector size
        u32 sector_size = 512;
        if (((data[106] >> 12) & 0b1101) == 0b1000) {
            sector_size = data[117] | (data[118] << 16);
        }

        // read device model
        char model[41];
        for (int i = 0; i < 20; i++) {
            model[2 * i + 0] = data[27 + i] >> 8;
            model[2 * i + 1] = data[27 + i] & 0xFF;
        }
        for (int i = 39; i >= 0; i--) {
            if (model[i] != ' ') {
                break;
            }
            model[i] = 0;
        }
        model[40] = 0;

        dprint("Found ATA device '");
        dprint(model);
        dprint("' (");
        dprintint(sector_count * sector_size / 1024 / 1024);
        dprintln(" MB)");

		ata_device_t* device = (ata_device_t*)kmalloc(sizeof(ata_device_t));
		if (device == NULL) {
			dprintln("ATA device: failed to allocate memory");
			continue;
		}

        device->device.read_sectors = ata_read_sectors;
        device->device.write_sectors = ata_write_sectors;
        device->device.sector_count = sector_count;
        device->device.sector_size = sector_size;
        device->device.model = device->model;
        device->io_base = io_base;
        device->control_base = control_base;
        device->index = i;
        strcpy(device->model, model);
		storage_device_add((storage_device_t*)device);
    }
}

void ata_controller_init() {
    // FIXME: this should be done through PCI
    ata_bus_init(0x1F0, 0x3F6);
    ata_bus_init(0x170, 0x376);
}
