#pragma once

#include "../types.h"
#include <stdbool.h>

typedef struct storage_device {
    bool (*read_sectors)(void* self, void* buffer, u64 sector, u64 count);
    bool (*write_sectors)(void* self, const void* buffer, u64 sector, u64 count);
    u64 sector_count;
    u32 sector_size;
    const char* model;
    struct storage_device** partitions;
    u32 partition_count;
} storage_device_t;

// Initialize all storage devices
// Returns the number of storage devices found
int storage_device_init();

// Add a storage device to the list of storage devices
void storage_device_add(storage_device_t* device);

extern int g_storage_device_count;
extern storage_device_t** g_storage_devices;
