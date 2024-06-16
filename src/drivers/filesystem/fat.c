// FAT support

#include "fat.h"
#include "../types.h"

// Global variables
unsigned int fat_type;
unsigned int first_fat_sector;
unsigned int first_data_sector;
unsigned int total_clusters;

// This actually makes the fat stuff work.
void FAT_Init(){
    return 0; // <-- THIS IS JUST A PLACEHOLDER!
}

// Open a file
void FAT_Open(file){
    return 0; // <-- THIS IS JUST A PLACEHOLDER!
}

// Reads a file
void FAT_Read(file){
    return 0; // <-- THIS IS JUST A PLACEHOLDER!
}

// Writes a file
void FAT_Write(file){
    return 0; // <-- THIS IS JUST A PLACEHOLDER!
}

// Bootsector stuff! (donated from the OSDev Wiki)
typedef struct fat_extBS_32
{
	// Extended fat32 stuff
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
 
}__attribute__((packed)) fat_extBS_32_t;
 
typedef struct fat_extBS_16
{
	// Extended fat12 and fat16 stuff
	u8			bios_drive_num;
	u8			reserved1;
	u8			boot_signature;
	u32			volume_id;
	u8			volume_label[11];
	u8			fat_type_label[8];
 
}__attribute__((packed)) fat_extBS_16_t;
 
typedef struct fat_BS
{
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
 
	// this will be cast to it's specific type once the driver actually knows what type of FAT this is.
	u8			extended_section[54];
 
}__attribute__((packed)) fat_BS_t;