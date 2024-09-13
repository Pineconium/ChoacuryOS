#include "cd.h"

#include "../../shell.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"
#include "../../../gui/desktop.h"

int shell_cd_command(int argc, const char** argv) {
    if (argc != 2) {
        return 2;
    }

    if (s_fat_fs == NULL) {
        term_write("ERROR: FAT filesystem not initialized\n", TC_LRED);
        return 2;
    }

    const char* new_dir_path = argv[1];
    char full_path[256];
    int full_path_len = 0;

    // Handle ".."
    if (strcmp(new_dir_path, "..") == 0) {
        if (strcmp(currentDir, "root") == 0 || strcmp(currentDir, "") == 0) {
        } else {
            char* last_slash = find_last_slash(currentDir);
            if (last_slash != NULL) {
                *last_slash = '\0';
            } else {
                strcpy(currentDir, "root");  // If somehow beyond root, reset to root
            }
        }
        return;
    }

    if (strcmp(currentDir, "root") != 0) {
        while (currentDir[full_path_len] != '\0') {
            full_path[full_path_len] = currentDir[full_path_len];
            full_path_len++;
        }

        full_path[full_path_len] = '/';
        full_path_len++;
    }

    int i = 0;
    while (new_dir_path[i] != '\0') {
        full_path[full_path_len] = new_dir_path[i];
        full_path_len++;
        i++;
    }

    full_path[full_path_len] = '\0';

    FAT_file_t* new_dir = FAT_OpenAbsolute(s_fat_fs, full_path);
    if (new_dir == NULL) {
        term_write("ERROR: Directory not found: '", TC_LRED);
        term_write(full_path, TC_LRED);
        term_write("'\n", TC_LRED);
        return 1;
    }

    if (!FAT_IsDirectory(new_dir)) {
        term_write("ERROR: Not a directory: '", TC_LRED);
        term_write(full_path, TC_LRED);
        term_write("'\n", TC_LRED);
        FAT_Close(new_dir);
        return 1;
    }

    // Update the current dir
    strcpy(currentDir, full_path);

    term_write(currentDir, TC_WHITE);
    term_write("\n", TC_WHITE);

    FAT_Close(new_dir);

    return 0;
}