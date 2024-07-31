#include "pmm.h"
#include "../kernel/panic.h"

u64 g_total_pmm_bytes;

void pmm_init(const multiboot_info_t* multiboot_info) {
    if (!(multiboot_info->flags & MULTIBOOT_INFO_MEM_MAP)) {
        panic("Multiboot info does not contain memory map\n");
    }

    g_total_pmm_bytes = 0;

    for (uint32_t offset = 0; offset < multiboot_info->mmap_length;) {
        const multiboot_memory_map_t* entry = (const multiboot_memory_map_t*)(multiboot_info->mmap_addr + offset);
        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
            g_total_pmm_bytes += entry->len;
        }
        offset += entry->size + sizeof(entry->size);
    }
}
