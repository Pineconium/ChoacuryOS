#pragma once

#include "../types.h"
#include <stdbool.h>

#define MAX_STORAGE_DEVICES 16

typedef struct {
    bool (*read_sectors)(void* self, void* buffer, u64 sector, u64 count);
    bool (*write_sectors)(void* self, const void* buffer, u64 sector, u64 count);
    u64 sector_count;
    u32 sector_size;
} storage_device_t;

// Initialize all storage devices
// Returns the number of storage devices found
int storage_device_init();

extern int g_storage_device_count;
extern storage_device_t* g_storage_devices[MAX_STORAGE_DEVICES];
