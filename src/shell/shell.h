#pragma once

void shell_start();

#include "../drivers/filesystem/fat.h"

extern FAT_filesystem_t* s_fat_fs;
extern char currentDir[256];

// PSF1
#include "../drivers/vbe.h"
extern PSF1_FONT* font;