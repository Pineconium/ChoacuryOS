#pragma once

#include "device.h"

bool partition_probe_gpt(storage_device_t*, u8 buffer[512]);
