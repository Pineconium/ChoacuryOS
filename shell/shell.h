#pragma once

void shell_start();

#include "../drivers/filesystem/fat.h"

extern FAT_filesystem_t* s_fat_fs;
extern char currentDir[256];