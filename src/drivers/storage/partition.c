#include "../debug.h"
#include "../utils.h"
#include "device.h"
#include "gpt.h"
#include "partition.h"
#include <kernel/panic.h>
#include <memory/kmalloc.h>

typedef struct {
    storage_device_t device;
    storage_device_t* disk;
    u64 first_sector;
} partition_device_t;

static bool partition_read_sectors(void* self, void* buffer, u64 sector, u64 count) {
    partition_device_t* device = (partition_device_t*)self;
    if (sector + count > device->device.sector_count) {
        dprintln("Partition read: sector out of range");
        return false;
    }
    return device->disk->read_sectors(device->disk, buffer, device->first_sector + sector, count);
}

static bool partition_write_sectors(void* self, const void* buffer, u64 sector, u64 count) {
    partition_device_t* device = (partition_device_t*)self;
    if (sector + count > device->device.sector_count) {
        dprintln("Partition write: sector out of range");
        return false;
    }
    return device->disk->write_sectors(device, buffer, device->first_sector + sector, count);
}

bool partition_probe(storage_device_t* device) {
    u8 buffer[512];

    // Read the first sector of the device
    if (!device->read_sectors(device, buffer, 1, 1)) {
        dprint("\e[33mFailed to read sector 1 of '");
        dprint(device->model);
        dprintln("'\e[m");
        return false;
    }

    if (memcmp(buffer, "EFI PART", 8) == 0) {
        return partition_probe_gpt(device, buffer);
    }

    dprint("\e[33mNo known partition table found on '");
    dprint(device->model);
    dprintln("'\e[m");

    return false;
}

void partition_add_to_device(storage_device_t* disk, u64 first_sector, u64 last_sector, const char* name) {
    partition_device_t* partition = (partition_device_t*)kmalloc(sizeof(partition_device_t));
    if (partition == NULL) {
        panic("Failed to allocate memory for partition");
    }

    char* model = (char*)kmalloc(strlen(name) + 1);
    if (model == NULL) {
        panic("Failed to allocate memory for partition model");
    }
    strcpy(model, name);

    partition->device.read_sectors = partition_read_sectors;
    partition->device.write_sectors = partition_write_sectors;
    partition->device.sector_count = last_sector - first_sector + 1;
    partition->device.sector_size = disk->sector_size;
    partition->device.model = model;
    partition->disk = disk;
    partition->first_sector = first_sector;

    storage_device_t** new_partitions = (storage_device_t**)kmalloc(sizeof(storage_device_t*) * (disk->partition_count + 1));
    if (new_partitions == NULL) {
        panic("Failed to allocate memory for partitions");
    }
    memcpy(new_partitions, disk->partitions, sizeof(storage_device_t*) * disk->partition_count);
    kfree(disk->partitions);
    new_partitions[disk->partition_count++] = (storage_device_t*)partition;
    disk->partitions = new_partitions;
}
