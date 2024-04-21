#include "ata.h"
#include "device.h"
#include <stddef.h>

int g_storage_device_count ;
storage_device_t* g_storage_devices[MAX_STORAGE_DEVICES];

int storage_device_init() {
    g_storage_device_count = 0;
    for (int i = 0; i < MAX_STORAGE_DEVICES; i++) {
        g_storage_devices[i] = NULL;
    }

    ata_controller_init();

    return g_storage_device_count;
}
