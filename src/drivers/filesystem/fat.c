// FAT support

#include "fat.h"
#include "../debug.h"
#include "../types.h"
#include "../utils.h"
#include "../vga.h"
#include "../../memory/kmalloc.h"
#include "../../kernel/panic.h"
#include "../../shell/terminal.h"

// Bootsector stuff! (donated from the OSDev Wiki)

// Extended fat32 stuff
typedef struct FAT_ext32 {
	u32			table_size_32;
	u16			extended_flags;
	u16			fat_version;
	u32			root_cluster;
	u16			fat_info;
	u16			backup_BS_sector;
	u8 			reserved_0[12];
	u8			drive_number;
	u8 			reserved_1;
	u8			boot_signature;
	u32 		volume_id;
	u8			volume_label[11];
	u8			fat_type_label[8];
}__attribute__((packed)) FAT_ext32_t;

// Extended fat12 and fat16 stuff
typedef struct FAT_ext16 {
	u8			bios_drive_num;
	u8			reserved1;
	u8			boot_signature;
	u32			volume_id;
	u8			volume_label[11];
	u8			fat_type_label[8];
}__attribute__((packed)) FAT_ext16_t;

typedef struct FAT_bs {
	u8 			bootjmp[3];
	u8 			oem_name[8];
	u16 		bytes_per_sector;
	u8			sectors_per_cluster;
	u16			reserved_sector_count;
	u8			table_count;
	u16			root_entry_count;
	u16			total_sectors_16;
	u8			media_type;
	u16			table_size_16;
	u16			sectors_per_track;
	u16			head_side_count;
	u32 		hidden_sector_count;
	u32 		total_sectors_32;

	union
	{
		FAT_ext16_t ext_bs_16;
		FAT_ext32_t ext_bs_32;
	};
}__attribute__((packed)) FAT_bs_t;

typedef struct FAT_date {
	uint16_t day   : 5;
	uint16_t month : 4;
	uint16_t year  : 7;
} FAT_date_t;

typedef struct FAT_time {
	uint16_t second : 5;
	uint16_t minute : 6;
	uint16_t hour   : 5;
} FAT_time_t;

typedef struct FAT_dir_entry {
	uint8_t    name[11];
	uint8_t    attr;
	uint8_t    ntres;
	uint8_t    creation_time_hundreth;
	FAT_time_t creation_time;
	FAT_date_t creation_date;
	FAT_date_t last_access_date;
	uint16_t   first_cluster_hi;
	FAT_time_t write_time;
	FAT_date_t write_date;
	uint16_t   first_cluster_lo;
	uint32_t   file_size;
} __attribute__((packed)) FAT_dir_entry_t;

static bool is_power_of_two(uint32_t value) {
	if (value == 0)
		return false;
	return (value & (value - 1)) == 0;
}

static int min(int a, int b) {
	return a < b ? a : b;
}
static uint32_t allocate_new_cluster(FAT_filesystem_t* filesystem, uint32_t previous_cluster);
static void update_file_size_in_dir_entry(FAT_file_t* file);
static void read_entry_name(const FAT_dir_entry_t* entry, char* buffer) {
	int len = 0;

	// main part of name without trailing spaces
	memcpy(buffer, entry->name, 8);
	for (len = 8; len > 0; len--) {
		if (buffer[len - 1] != ' ') {
			break;
		}
	}

	// add file extension
	if (entry->name[8] != ' ') {
		buffer[len++] = '.';
		for (int j = 8; j < 11; j++) {
			if (entry->name[j] == ' ') {
				break;
			}
			buffer[len++] = entry->name[j];
		}
	}

	buffer[len] = 0;
}

// This actually makes the fat stuff work.
FAT_filesystem_t* FAT_Init(storage_device_t* storage_device) {
	uint8_t* sector_buffer = kmalloc(storage_device->sector_size);
	if (sector_buffer == NULL)
		return NULL;

	if (!storage_device->read_sectors(storage_device, sector_buffer, 0, 1))
		goto error_label;

	const FAT_bs_t* bs = (const FAT_bs_t*)sector_buffer;

	// Jump code must be of form 0xEB??90 or 0xE9????
	if (!((bs->bootjmp[0] == 0xEB && bs->bootjmp[2] == 0x90) || (bs->bootjmp[0] == 0xE9)))
		goto error_label;

	if (!is_power_of_two(bs->bytes_per_sector) || bs->bytes_per_sector < 512 || bs->bytes_per_sector > 4096)
		goto error_label;

	if (!is_power_of_two(bs->sectors_per_cluster) || bs->sectors_per_cluster > 128)
		goto error_label;

	if (bs->reserved_sector_count == 0)
		goto error_label;

	if (bs->table_count == 0)
		goto error_label;

	switch (bs->media_type)
	{
		case 0xF0: case 0xF8: case 0xF9:
		case 0xFA: case 0xFB: case 0xFC:
		case 0xFD: case 0xFE: case 0xFF:
			break;
		default:
			goto error_label;
	}

	uint32_t first_fat_sector = bs->reserved_sector_count;
	uint32_t fat_sector_count = bs->table_size_16 ? bs->table_size_16 : bs->ext_bs_32.table_size_32;

	uint32_t root_sector_count = (bs->root_entry_count * 32 + (bs->bytes_per_sector - 1)) / bs->bytes_per_sector;
	uint32_t total_sector_count = bs->total_sectors_16 ? bs->total_sectors_16 : bs->total_sectors_32;

	uint32_t first_data_sector = bs->reserved_sector_count + (bs->table_count * fat_sector_count) + root_sector_count;

	uint32_t data_sector_count = total_sector_count - first_data_sector;
	uint32_t cluster_count = data_sector_count / bs->sectors_per_cluster;

	uint8_t* fat_sector_buffer = kmalloc(bs->bytes_per_sector * 2);
	if (fat_sector_buffer == NULL) {
		goto error_label;
	}

	FAT_filesystem_t* result = kmalloc(sizeof(FAT_filesystem_t));
	if (result == NULL) {
		kfree(fat_sector_buffer);
		goto error_label;
	}

	result->storage_device = storage_device;
	result->fat_type = (cluster_count < 4085) ? FAT12 : (cluster_count < 65525) ? FAT16 : FAT32;

	result->root_file.filesystem = result;
	result->root_file.is_directory = true;

	switch (result->fat_type)
	{
		case FAT12:
		case FAT16:
			result->fat16.root_sector = bs->reserved_sector_count + (bs->table_count * fat_sector_count);
			result->fat16.root_entry_count = bs->root_entry_count;
			break;
		case FAT32:
			result->root_file.first_cluster = bs->ext_bs_32.root_cluster;
			break;
		default:
			panic("NOT REACHED");
	}

	result->bytes_per_sector = bs->bytes_per_sector;
	result->sectors_per_cluster = bs->sectors_per_cluster;

	result->first_data_sector = first_data_sector;
	result->first_fat_sector = first_fat_sector;

	result->cluster_count = cluster_count;

	result->sector_buffer = fat_sector_buffer;
	result->sector_buffer_sector = 0;

	kfree(sector_buffer);
	return result;

error_label:
	kfree(sector_buffer);
	return NULL;
}

FAT_file_t* FAT_OpenAbsolute(FAT_filesystem_t* filesystem, const char* path) {
	FAT_file_t* file = &filesystem->root_file;

	char buffer[512];
	while (*path) {
		size_t i = 0;
		while (*path && *path != '/') {
			buffer[i++] = *path;
			path++;
		}
		if (*path) {
			path++;
		}
		buffer[i] = '\0';

		if (i == 0) {
			continue;
		}

		FAT_file_t* temp = file;
		file = FAT_Open(file, buffer);
		FAT_Close(temp);

		if (file == NULL) {
			return NULL;
		}
	}

	return file;
}

static bool for_each_fat_dir_entry(const uint8_t* buffer, size_t buffer_len, void* data, bool(*callback)(const FAT_dir_entry_t*, void*)) {
	for (uint32_t offset = 0; offset < buffer_len; offset += sizeof(FAT_dir_entry_t)) {
		const FAT_dir_entry_t* entry = (const FAT_dir_entry_t*)(buffer + offset);

		// Last entry
		if (entry->name[0] == 0) {
			return true;
		}

		// Unused entry
		if (entry->name[0] == 0xE5) {
			continue;
		}

		// Skip long name entries
		if ((entry->attr & 0x3F) == 0x0F) {
			continue;
		}

		if (callback(entry, data)) {
			return true;
		}
	}

	return false;
}

typedef struct search_info {
	const char* name;
	FAT_file_t* file;
	bool found;
} search_info_t;

static bool find_fat_dir_entry_by_name(const FAT_dir_entry_t* entry, void* data) {
	char entry_name[13];
	read_entry_name(entry, entry_name);

	search_info_t* info = (search_info_t*)data;
	if (strlen(info->name) != strlen(entry_name)) {
		return false;
	}
	for (size_t i = 0; info->name[i]; i++) {
		if (tolower(entry_name[i]) != tolower(info->name[i])) {
			return false;
		}
	}

	info->file->first_cluster = ((uint32_t)entry->first_cluster_hi << 16) | entry->first_cluster_lo;
	info->file->file_size = entry->file_size;
	info->file->is_directory = !!(entry->attr & 0x10);
	info->found = true;

	return true;
}

static bool is_valid_cluster(FAT_filesystem_t* filesystem, uint32_t cluster) {
	if (cluster < 2 || cluster >= filesystem->cluster_count) {
		return false;
	}
	return true;
}

static uint32_t get_next_cluster(FAT_filesystem_t* filesystem, uint32_t cluster) {
	if (!is_valid_cluster(filesystem, cluster)) {
		panic("NOT REACHED");
	}

	uint32_t fat_byte_offset;
	uint32_t ent_offset;

	switch (filesystem->fat_type)
	{
		case FAT12:
			fat_byte_offset = cluster + (cluster / 2);
			ent_offset = fat_byte_offset % filesystem->bytes_per_sector;
			break;
		case FAT16:
			fat_byte_offset = cluster * sizeof(uint16_t);
			ent_offset = (fat_byte_offset % filesystem->bytes_per_sector) / sizeof(uint16_t);
			break;
		case FAT32:
			fat_byte_offset = cluster * sizeof(uint32_t);
			ent_offset = (fat_byte_offset % filesystem->bytes_per_sector) / sizeof(uint32_t);
			break;
		default:
			panic("NOT REACHED");
	}

	uint32_t target_sector = filesystem->first_fat_sector + fat_byte_offset / filesystem->bytes_per_sector;
	if (filesystem->sector_buffer_sector != target_sector) {
		if (!filesystem->storage_device->read_sectors(filesystem->storage_device, filesystem->sector_buffer, target_sector, 2)) {
			dprintln("could not read file allocation table");
			return 0;
		}
		filesystem->sector_buffer_sector = target_sector;
	}

	switch (filesystem->fat_type)
	{
		case FAT12:
			uint16_t next = (filesystem->sector_buffer[ent_offset + 1] << 8) | filesystem->sector_buffer[ent_offset];
			return cluster % 2 ? next >> 4 : next & 0xFFF;
		case FAT16:
			return ((uint16_t*)filesystem->sector_buffer)[ent_offset];
		case FAT32:
			return ((uint32_t*)filesystem->sector_buffer)[ent_offset];
	}

	panic("NOT REACHED");
}

FAT_file_t* FAT_Open(FAT_file_t* parent, const char* name) {
	FAT_filesystem_t* filesystem = parent->filesystem;

	if (!parent->is_directory) {
		dprintln("trying to open file from non-directory");
		return NULL;
	}

	if (filesystem->bytes_per_sector != filesystem->storage_device->sector_size) {
		dprintln("FAT sector size does not match with its underlying storage, not supported");
		return NULL;
	}

	FAT_file_t* file = kmalloc(sizeof(FAT_file_t));
	if (file == NULL) {
		dprintln("could not allocate memory for FAT file");
		return NULL;
	}
	file->filesystem = filesystem;
	file->first_cluster = 0;

	search_info_t info;
	info.name = name;
	info.file = file;
	info.found = false;

	if ((filesystem->fat_type == FAT12 || filesystem->fat_type == FAT16) && parent == &filesystem->root_file) {
		uint8_t* buffer = kmalloc(filesystem->bytes_per_sector);
		if (buffer == NULL) {
			dprintln("could not allocate memory for FAT sector");
			kfree(file);
			return NULL;
		}

		uint32_t entries_per_sector = filesystem->bytes_per_sector / sizeof(FAT_dir_entry_t);
		for (uint32_t i = 0; i < filesystem->fat16.root_entry_count; i++) {
			uint32_t sector = filesystem->fat16.root_sector + (i / entries_per_sector);
			if (!filesystem->storage_device->read_sectors(filesystem->storage_device, buffer, sector, 1)) {
				dprintln("could not read root directory");
				break;
			}

			uint32_t buffer_len = min(filesystem->bytes_per_sector, (filesystem->fat16.root_entry_count - i) * sizeof(FAT_dir_entry_t));
			if (for_each_fat_dir_entry(buffer, buffer_len, &info, find_fat_dir_entry_by_name)) {
				break;
			}
		}

		kfree(buffer);
	}
	else {
		uint32_t cluster_size = filesystem->bytes_per_sector * filesystem->sectors_per_cluster;
		uint8_t* buffer = kmalloc(cluster_size);
		if (buffer == NULL) {
			dprintln("could not allocate memory for FAT cluster");
			kfree(file);
			return NULL;
		}

		uint32_t cluster = parent->first_cluster;
		while (is_valid_cluster(filesystem, cluster)) {
			uint32_t cluster_start_sector = ((cluster - 2) * filesystem->sectors_per_cluster) + filesystem->first_data_sector;
			if (!filesystem->storage_device->read_sectors(filesystem->storage_device, buffer, cluster_start_sector, filesystem->sectors_per_cluster)) {
				dprintln("could not read FAT cluster");
				break;
			}

			if (for_each_fat_dir_entry(buffer, cluster_size, &info, find_fat_dir_entry_by_name)) {
				break;
			}
		}

		kfree(buffer);
	}

	if (info.found) {
		info.file->first_cluster &= 0xFFFF;
		return info.file;
	}

	kfree(file);
	return NULL;
}

void FAT_Close(FAT_file_t* file) {
	// Root file cannot be freed as that is not allocated with kmalloc
	if (file != &file->filesystem->root_file) {
		kfree(file);
	}
}

size_t FAT_Read(FAT_file_t* file, size_t offset, void* dest_buffer, size_t buffer_len) {
	FAT_filesystem_t* filesystem = file->filesystem;

	if (file->is_directory) {
		dprintln("trying to read from directory");
		return 0;
	}

	if (offset >= file->file_size) {
		return 0;
	}

	if (offset + buffer_len > file->file_size) {
		buffer_len = file->file_size - offset;
	}

	if (filesystem->bytes_per_sector != filesystem->storage_device->sector_size) {
		dprintln("FAT sector size does not match with its underlying storage, not supported");
		return 0;
	}

	uint32_t cluster_size = filesystem->bytes_per_sector * filesystem->sectors_per_cluster;

	uint8_t* buffer = kmalloc(cluster_size);
	if (buffer == NULL) {
		dprintln("could not allocate memory for FAT cluster");
	}

	uint32_t cluster = file->first_cluster;
	for (uint32_t i = 0; i < offset / cluster_size; i++) {
		if (!is_valid_cluster(filesystem, cluster)) {
			dprintln("corrupted FAT file, no allocated cluster");
			kfree(buffer);
			return 0;
		}
		cluster = get_next_cluster(filesystem, cluster);
	}

	uint32_t bytes_copied = 0;
	while (bytes_copied < buffer_len) {
		if (!is_valid_cluster(filesystem, cluster)) {
			dprintln("corrupted FAT file, no allocated cluster");
			break;
		}

		uint32_t cluster_offset = offset % cluster_size;

		uint32_t cluster_start_sector = ((cluster - 2) * filesystem->sectors_per_cluster) + filesystem->first_data_sector;
		if (!filesystem->storage_device->read_sectors(filesystem->storage_device, buffer, cluster_start_sector, filesystem->sectors_per_cluster)) {
			dprintln("could not read file data directory");
			break;
		}

		uint32_t to_copy = min(cluster_size - cluster_offset, buffer_len);
		memcpy((uint8_t*)dest_buffer + bytes_copied, buffer + cluster_offset, to_copy);

		cluster = get_next_cluster(filesystem, cluster);

		bytes_copied += to_copy;
		offset += to_copy;
	}

	kfree(buffer);
	return bytes_copied;
}

typedef struct list_info {
	char** names;
	size_t name_count;
} list_info_t;

static bool append_fat_entry_name(const FAT_dir_entry_t* entry, void* data) {
	list_info_t* info = (list_info_t*)data;

	char** new_names = kmalloc((info->name_count + 1) * sizeof(char*));
	if (new_names == NULL) {
		return true;
	}

	char* entry_name = kmalloc(13);
	if (entry_name == NULL) {
		kfree(new_names);
		return true;
	}
	read_entry_name(entry, entry_name);

	for (size_t i = 0; i < info->name_count; i++) {
		new_names[i] = info->names[i];
	}
	new_names[info->name_count] = entry_name;

	kfree(info->names);
	info->names = new_names;
	info->name_count++;

	return false;
}

size_t FAT_ListFiles(FAT_file_t* parent, char*** names_output) {
	FAT_filesystem_t* filesystem = parent->filesystem;

	if (!parent->is_directory) {
		dprintln("trying to list files from non-directory");
		return 0;
	}

	if (filesystem->bytes_per_sector != filesystem->storage_device->sector_size) {
		dprintln("FAT sector size does not match with its underlying storage, not supported");
		return 0;
	}

	list_info_t info;
	info.names = NULL;
	info.name_count = 0;

	if ((filesystem->fat_type == FAT12 || filesystem->fat_type == FAT16) && parent == &filesystem->root_file) {
		uint8_t* buffer = kmalloc(filesystem->bytes_per_sector);
		if (buffer == NULL) {
			dprintln("could not allocate memory for FAT sector");
			return 0;
		}

		uint32_t root_total_bytes = filesystem->fat16.root_entry_count * sizeof(FAT_dir_entry_t);
		uint32_t root_bytes_done = 0;
		for (uint32_t i = 0; root_bytes_done < root_total_bytes; i++) {
			if (!filesystem->storage_device->read_sectors(filesystem->storage_device, buffer, filesystem->fat16.root_sector + i, 1)) {
				dprintln("could not read root directory");
				break;
			}

			uint32_t buffer_len = min(filesystem->bytes_per_sector, root_total_bytes - root_bytes_done);
			if (for_each_fat_dir_entry(buffer, buffer_len, &info, append_fat_entry_name)) {
				break;
			}

			root_bytes_done += buffer_len;
		}

		kfree(buffer);
	}
	else {
		uint8_t* buffer = kmalloc(filesystem->bytes_per_sector * filesystem->sectors_per_cluster);
		if (buffer == NULL) {
			dprintln("could not allocate memory for FAT cluster");
			return 0;
		}

		uint32_t cluster = parent->first_cluster;
		while (is_valid_cluster(filesystem, cluster)) {
			uint32_t cluster_start_sector = ((cluster - 2) * filesystem->sectors_per_cluster) + filesystem->first_data_sector;
			if (!filesystem->storage_device->read_sectors(filesystem->storage_device, buffer, cluster_start_sector, filesystem->sectors_per_cluster)) {
				dprintln("could not read FAT cluster");
				break;
			}

			u32 cluster_size = filesystem->bytes_per_sector * filesystem->sectors_per_cluster;
			if (for_each_fat_dir_entry(buffer, cluster_size, &info, append_fat_entry_name)) {
				break;
			}

			cluster = get_next_cluster(filesystem, cluster);
		}

		kfree(buffer);
	}

	*names_output = info.names;
	return info.name_count;
}
bool FAT_IsDirectory(FAT_file_t* file) {
	if (file == NULL) {
		return false; // Null pointer check
	}

	return file->is_directory;
}
//todo implement this
//gl
size_t FAT_Write(FAT_file_t* file, size_t offset, const void* src_buffer, size_t buffer_len) {
    FAT_filesystem_t* filesystem = file->filesystem;

    if (file->is_directory) {
        dprintln("trying to write to a directory");
        return 0;
    }

    if (filesystem->bytes_per_sector != filesystem->storage_device->sector_size) {
        dprintln("FAT sector size does not match with its underlying storage, not supported");
        return 0;
    }

    uint32_t cluster_size = filesystem->bytes_per_sector * filesystem->sectors_per_cluster;

    // Buffer for cluster-level operations
    uint8_t* buffer = kmalloc(cluster_size);
    if (buffer == NULL) {
        dprintln("could not allocate memory for FAT cluster");
        return 0;
    }

    uint32_t cluster = file->first_cluster;
    uint32_t cluster_offset = offset / cluster_size;
    size_t bytes_written = 0;

    // Move to the correct cluster
    for (uint32_t i = 0; i < cluster_offset; i++) {
        if (!is_valid_cluster(filesystem, cluster)) {
            cluster = get_next_cluster(filesystem, cluster);
            if (cluster == 0) {
                dprintln("could not find valid cluster");
                kfree(buffer);
                return bytes_written;
            }
        }
    }

    while (bytes_written < buffer_len) {
        if (!is_valid_cluster(filesystem, cluster)) {
            dprintln("invalid cluster");
            break;
        }

        uint32_t cluster_start_sector = ((cluster - 2) * filesystem->sectors_per_cluster) + filesystem->first_data_sector;

        // Calculate the offset within the cluster
        uint32_t cluster_start_offset = offset % cluster_size;
        uint32_t write_len = min(cluster_size - cluster_start_offset, buffer_len - bytes_written);

        // Read current cluster content if not writing from start
        if (cluster_start_offset != 0 || write_len < cluster_size) {
            if (!filesystem->storage_device->read_sectors(filesystem->storage_device, buffer, cluster_start_sector, filesystem->sectors_per_cluster)) {
                dprintln("could not read FAT cluster before writing");
                break;
            }
        }

        // Copy data from source buffer to cluster buffer
        memcpy(buffer + cluster_start_offset, (uint8_t*)src_buffer + bytes_written, write_len);

        // Write back the updated cluster content to the disk
        if (!filesystem->storage_device->write_sectors(filesystem->storage_device, buffer, cluster_start_sector, filesystem->sectors_per_cluster)) {
            dprintln("could not write to FAT cluster");
            break;
        }

        bytes_written += write_len;
        offset += write_len;

        // Move to the next cluster
        if (bytes_written < buffer_len) {
            uint32_t next_cluster = get_next_cluster(filesystem, cluster);
            if (next_cluster == 0) {
                // Allocate a new cluster if necessary
                // (This part of the code assumes you have a method to allocate a new cluster)
                next_cluster = allocate_new_cluster(filesystem, cluster);
                if (next_cluster == 0) {
                    dprintln("could not allocate new cluster");
                    break;
                }
            }
            cluster = next_cluster;
        }
    }

    // Update the file size if it has increased
    if (offset > file->file_size) {
        file->file_size = offset;
        // (Update the directory entry to reflect the new size)
        update_file_size_in_dir_entry(file);
    }

    kfree(buffer);
    return bytes_written;
}
static uint32_t allocate_new_cluster(FAT_filesystem_t* filesystem, uint32_t previous_cluster) {
    uint32_t fat_byte_offset, ent_offset;
    uint32_t fat_sector, cluster;

    // Find a free cluster in the FAT table
    for (cluster = 2; cluster < filesystem->cluster_count; cluster++) {
        switch (filesystem->fat_type) {
            case FAT12:
                fat_byte_offset = cluster + (cluster / 2);
                ent_offset = fat_byte_offset % filesystem->bytes_per_sector;
                break;
            case FAT16:
                fat_byte_offset = cluster * sizeof(uint16_t);
                ent_offset = (fat_byte_offset % filesystem->bytes_per_sector) / sizeof(uint16_t);
                break;
            case FAT32:
                fat_byte_offset = cluster * sizeof(uint32_t);
                ent_offset = (fat_byte_offset % filesystem->bytes_per_sector) / sizeof(uint32_t);
                break;
            default:
                return 0;
        }

        fat_sector = filesystem->first_fat_sector + fat_byte_offset / filesystem->bytes_per_sector;
        if (filesystem->sector_buffer_sector != fat_sector) {
            if (!filesystem->storage_device->read_sectors(filesystem->storage_device, filesystem->sector_buffer, fat_sector, 1)) {
                return 0;
            }
            filesystem->sector_buffer_sector = fat_sector;
        }

        uint32_t cluster_value;
        switch (filesystem->fat_type) {
            case FAT12:
                cluster_value = (filesystem->sector_buffer[ent_offset + 1] << 8) | filesystem->sector_buffer[ent_offset];
                cluster_value = (cluster % 2 == 0) ? (cluster_value & 0xFFF) : (cluster_value >> 4);
                break;
            case FAT16:
                cluster_value = ((uint16_t*)filesystem->sector_buffer)[ent_offset];
                break;
            case FAT32:
                cluster_value = ((uint32_t*)filesystem->sector_buffer)[ent_offset] & 0x0FFFFFFF;
                break;
            default:
                return 0;
        }

        if (cluster_value == 0) { // Free cluster
            switch (filesystem->fat_type) {
                case FAT12:
                    if (cluster % 2 == 0) {
                        filesystem->sector_buffer[ent_offset] = 0xFF;
                        filesystem->sector_buffer[ent_offset + 1] = 0xF;
                    } else {
                        filesystem->sector_buffer[ent_offset] |= 0xF0;
                    }
                    break;
                case FAT16:
                    ((uint16_t*)filesystem->sector_buffer)[ent_offset] = 0xFFFF;
                    break;
                case FAT32:
                    ((uint32_t*)filesystem->sector_buffer)[ent_offset] = 0x0FFFFFFF;
                    break;
            }

            if (!filesystem->storage_device->write_sectors(filesystem->storage_device, filesystem->sector_buffer, fat_sector, 1)) {
                return 0;
            }

            if (previous_cluster != 0) {
                // Link the previous cluster to the new one
                uint32_t prev_fat_byte_offset, prev_ent_offset, prev_fat_sector;
                switch (filesystem->fat_type) {
                    case FAT12:
                        prev_fat_byte_offset = previous_cluster + (previous_cluster / 2);
                        prev_ent_offset = prev_fat_byte_offset % filesystem->bytes_per_sector;
                        break;
                    case FAT16:
                        prev_fat_byte_offset = previous_cluster * sizeof(uint16_t);
                        prev_ent_offset = (prev_fat_byte_offset % filesystem->bytes_per_sector) / sizeof(uint16_t);
                        break;
                    case FAT32:
                        prev_fat_byte_offset = previous_cluster * sizeof(uint32_t);
                        prev_ent_offset = (prev_fat_byte_offset % filesystem->bytes_per_sector) / sizeof(uint32_t);
                        break;
                    default:
                        return 0;
                }

                prev_fat_sector = filesystem->first_fat_sector + prev_fat_byte_offset / filesystem->bytes_per_sector;
                if (filesystem->sector_buffer_sector != prev_fat_sector) {
                    if (!filesystem->storage_device->read_sectors(filesystem->storage_device, filesystem->sector_buffer, prev_fat_sector, 1)) {
                        return 0;
                    }
                    filesystem->sector_buffer_sector = prev_fat_sector;
                }

                switch (filesystem->fat_type) {
                    case FAT12:
                        if (previous_cluster % 2 == 0) {
                            filesystem->sector_buffer[prev_ent_offset] = (uint8_t)(cluster & 0xFF);
                            filesystem->sector_buffer[prev_ent_offset + 1] = (filesystem->sector_buffer[prev_ent_offset + 1] & 0xF0) | (cluster >> 8);
                        } else {
                            filesystem->sector_buffer[prev_ent_offset] = (filesystem->sector_buffer[prev_ent_offset] & 0xF) | ((cluster & 0xF) << 4);
                            filesystem->sector_buffer[prev_ent_offset + 1] = (uint8_t)(cluster >> 4);
                        }
                        break;
                    case FAT16:
                        ((uint16_t*)filesystem->sector_buffer)[prev_ent_offset] = (uint16_t)cluster;
                        break;
                    case FAT32:
                        ((uint32_t*)filesystem->sector_buffer)[prev_ent_offset] = cluster & 0x0FFFFFFF;
                        break;
                }

                if (!filesystem->storage_device->write_sectors(filesystem->storage_device, filesystem->sector_buffer, prev_fat_sector, 1)) {
                    return 0;
                }
            }

            return cluster;
        }
    }

    return 0;
}
static void update_file_size_in_dir_entry(FAT_file_t* file) {
	FAT_filesystem_t* filesystem = file->filesystem;
	uint32_t cluster = file->first_cluster;

	uint8_t* buffer = kmalloc(filesystem->bytes_per_sector);
	if (buffer == NULL) {
		dprintln("could not allocate memory for FAT sector");
		return;
	}

	uint32_t cluster_size = filesystem->bytes_per_sector * filesystem->sectors_per_cluster;

	uint32_t cluster_start_sector = ((cluster - 2) * filesystem->sectors_per_cluster) + filesystem->first_data_sector;
	if (!filesystem->storage_device->read_sectors(filesystem->storage_device, buffer, cluster_start_sector, filesystem->sectors_per_cluster)) {
		dprintln("could not read FAT cluster");
		kfree(buffer);
		return;
	}

	FAT_dir_entry_t* entry = NULL;
	for (uint32_t offset = 0; offset < cluster_size; offset += sizeof(FAT_dir_entry_t)) {
		entry = (FAT_dir_entry_t*)(buffer + offset);

		if (entry->name[0] == 0x00) {
			break;
		}

		if (entry->first_cluster_lo == (file->first_cluster & 0xFFFF) &&
			entry->first_cluster_hi == (file->first_cluster >> 16)) {
			entry->file_size = file->file_size;
			break;
			}
	}

	if (entry) {
		if (!filesystem->storage_device->write_sectors(filesystem->storage_device, buffer, cluster_start_sector, filesystem->sectors_per_cluster)) {
			dprintln("could not update file size in directory entry");
		}
	}

	kfree(buffer);
}

