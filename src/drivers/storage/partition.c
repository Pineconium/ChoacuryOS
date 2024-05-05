#include "../debug.h"
#include "../utils.h"
#include "gpt.h"
#include "partition.h"

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
