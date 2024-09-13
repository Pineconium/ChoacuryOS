#include "mkdir.h"

#include "../../shell.h"
#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"
#include "../../../drivers/filesystem/fat.h"

int shell_mkdir_command(int argc, const char** argv) {
    if (argc > 2) {
        return 2;
    }
    if (s_fat_fs == NULL) {
        term_write("ERROR: Not FAT filesystem initialized\n", TC_LRED);
        return 1;
    }

    const char* path = (argc == 2) ? argv[1] : currentDir;

    term_write("TEMP! While Choacury DOES support file making, its currently a WIP. I'll be gone soon once I am finished.\n", TC_YELLO);

    /* PATH is the current directory. */
    // FAT_CreateDirectory(path, argv[2]);