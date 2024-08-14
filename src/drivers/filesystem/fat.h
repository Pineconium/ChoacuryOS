#pragma once

#include "../storage/device.h"

typedef enum FAT_type {
	FAT12,
	FAT16,
	FAT32
} FAT_type_t;

typedef struct FAT_file {
	struct FAT_filesystem* filesystem;
	uint32_t first_cluster;
	uint32_t file_size;
	bool is_directory;
} FAT_file_t;

typedef struct FAT_filesystem {
	storage_device_t* storage_device;

	FAT_type_t fat_type;

	// Only used in FAT12 and FAT16
	struct {
		uint32_t root_sector;
		uint32_t root_entry_count;
	} fat16;

	uint32_t bytes_per_sector;
	uint32_t sectors_per_cluster;

	uint32_t first_fat_sector;
	uint32_t first_data_sector;

	uint32_t cluster_count;

	uint8_t* sector_buffer;
	uint32_t sector_buffer_sector;

	FAT_file_t root_file;
} FAT_filesystem_t;

// Try to initialize FAT filesystem from storage device
//   on success:
//     return pointer to FAT filesystem
//   on error:
//     return NULL
FAT_filesystem_t* FAT_Init(storage_device_t*);

// Open file with absolute path from a FAT filesystem
// Path separator '/'s is used while parsing
//   on success
//     return a pointer to FAT file
//   file not found:
//     return NULL
//   on error:
//     return NULL
FAT_file_t* FAT_OpenAbsolute(FAT_filesystem_t*, const char* path);

// Look for a file with name from parent file
// Open files MUST be closed with FAT_Close
//   on success
//     return a pointer to FAT file
//   file not found:
//     return NULL
//   on error:
//     return NULL
FAT_file_t* FAT_Open(FAT_file_t* parent, const char* name);

// Close a FAT file
void FAT_Close(FAT_file_t* file);

// Read bytes from a FAT file from given offset
//   on success
//     return the number of bytes read
//   on end of file
//     return 0
//   on error:
//     return 0
size_t FAT_Read(FAT_file_t* file, size_t offset, void* buffer, size_t buffer_len);

// List all files from parent into the output names
// Output names is pointer to list of char pointers
// All entries in *names_output and *names_output itself MUST be deleted with kfree
//   always returns the number of entries in *names_output
size_t FAT_ListFiles(FAT_file_t* parent, char*** names_output);

// Check if the file is a directory
bool FAT_IsDirectory(FAT_file_t* file);
//void FAT_Write(file);
