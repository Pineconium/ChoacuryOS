#pragma once

#include "../drivers/types.h"
#include "../kernel/multiboot.h"

extern u64 g_total_pmm_bytes;

void pmm_init(const multiboot_info_t* multiboot_info);
