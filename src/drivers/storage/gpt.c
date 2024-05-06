#include "../debug.h"
#include "../utils.h"
#include "gpt.h"

struct gpt_header {
	u8 signature[8];
	u32 revision;
	u32 header_size;
	u32 header_crc32;
	u32 reserved;
	u64 current_lba;
	u64 backup_lba;
	u64 first_usable_lba;
	u64 last_usable_lba;
	u8 disk_guid[16];
	u64 partition_entry_lba;
	u32 num_partition_entries;
	u32 size_partition_entry;
	u32 partition_entry_array_crc32;
} __attribute__((packed));

struct gpt_partition_entry {
	u8 partition_type_guid[16];
	u8 unique_partition_guid[16];
	u64 first_lba;
	u64 last_lba;
	u64 attributes;
	u16 partition_name[36];
} __attribute__((packed));

bool partition_probe_gpt(storage_device_t* device, u8 buffer[512]) {
	struct gpt_header* gpt_header = (struct gpt_header*)buffer;
	if (memcmp(gpt_header->signature, "EFI PART", 8) != 0) {
		return false;
	}

	dprint("Found GPT on '");
	dprint(device->model);
	dprintln("'");

	// FIXME: Do error checking
	
	u8 partition_buffer[512];
	u32 last_partition_lba = 0;

	for (u32 i = 0; i < gpt_header->num_partition_entries; i++) {
		u32 byte_offset = gpt_header->partition_entry_lba * device->sector_size + i * gpt_header->size_partition_entry;
		if (byte_offset / device->sector_size != last_partition_lba) {
			if (!device->read_sectors(device, partition_buffer, byte_offset / device->sector_size, 1)) {
				dprint("\e[33mFailed to read partition entry ");
				dprintint(i);
				dprintln("\e[m");
				return false;
			}
			last_partition_lba = byte_offset / device->sector_size;
		}

		struct gpt_partition_entry* gpt_partition_entry = (struct gpt_partition_entry*)(partition_buffer + byte_offset % device->sector_size);
		u8 zero_guid[16] = { 0 };
		if (memcmp(gpt_partition_entry->partition_type_guid, zero_guid, 16) == 0) {
			continue;
		}

		u64 sectors = gpt_partition_entry->last_lba - gpt_partition_entry->first_lba + 1;
		u64 size = sectors * device->sector_size;

		dprint("  partition ");
		dprintint(i);
		dprintln(":");
	
		dprint("    size: ");
		dprintint(size / 1024 / 1024);
		dprintln(" MiB");
	
		dprint("    name: '");
		for (int j = 0; j < 36; j++) {
			dprintchar(gpt_partition_entry->partition_name[j]);
		}
		dprintln("'");

		// FIXME: Create partition device when we have dynamic memory allocation
	}

	return true;
}
