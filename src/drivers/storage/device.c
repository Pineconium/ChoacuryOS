#include "ata.h"
#include "device.h"
#include "partition.h"
#include <kernel/panic.h>
#include <memory/kmalloc.h>

static int g_storage_device_capacity;
int g_storage_device_count;
storage_device_t** g_storage_devices;

int storage_device_init() {
    g_storage_device_count = 0;

	g_storage_device_capacity = 16;
	g_storage_devices = (storage_device_t**)kmalloc(sizeof(storage_device_t*) * g_storage_device_capacity);
	if (g_storage_devices == NULL) {
		panic("Failed to allocate memory for storage devices");
	}

    ata_controller_init();

	for (int i = 0; i < g_storage_device_count; i++) {
		partition_probe(g_storage_devices[i]);
	}

    return g_storage_device_count;
}

void storage_device_add(storage_device_t* device) {
	// Resize the list if necessary
	if (g_storage_device_count == g_storage_device_capacity) {
		int new_capacity = g_storage_device_capacity * 2;
		storage_device_t** new_devices = (storage_device_t**)kmalloc(sizeof(storage_device_t*) * new_capacity);
		if (new_devices == NULL) {
			panic("Failed to allocate memory for storage devices");
		}
		for (int i = 0; i < g_storage_device_count; i++) {
			new_devices[i] = g_storage_devices[i];
		}
		kfree(g_storage_devices);
		g_storage_devices = new_devices;
		g_storage_device_capacity = new_capacity;
	}

	// Add the device to the list
	g_storage_devices[g_storage_device_count++] = device;
}
