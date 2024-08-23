#include "cat.h"

#include "../../shell.h"
#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"
#include "../../../gui/desktop.h"
#include "../../../drivers/filesystem/fat.h"

int shell_cat_command(int argc, const char** argv) {
    if (argc != 2) {
        return 2;
    }
    if (s_fat_fs == NULL) {
        term_write("ERROR: Not FAT filesystem initialized\n", TC_LRED);
        return;
    }

    FAT_file_t* file = FAT_OpenAbsolute(s_fat_fs, argv[1]);
    if (file == NULL) {
        term_write("ERROR: Not found: '", TC_LRED);
        term_write(argv[1], TC_LRED);
        term_write("'\n", TC_LRED);
        FAT_Close(file);
        return;
    }

    char buffer[513];
    size_t total_read = 0;
    while (true) {
        size_t nread = FAT_Read(file, total_read, buffer, sizeof(buffer) - 1);
        if (nread == 0) {
            break;
        }

        buffer[nread] = '\0';
        term_write(buffer, TC_WHITE);

        total_read += nread;
    }

    FAT_Close(file);
    return 0;
}