#pragma once

#include "device.h"

bool partition_probe(storage_device_t*);
void partition_add_to_device(storage_device_t* disk, u64 first_sector, u64 last_sector, const char* name);
