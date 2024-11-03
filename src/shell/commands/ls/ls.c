#include "ls.h"

#include "../../shell.h"
#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"
#include "../../../drivers/filesystem/fat.h"

int shell_ls_command(int argc, const char** argv) {
    if (argc > 2) {
        return 2;
    }
    if (s_fat_fs == NULL) {
        term_write("ERROR: Not FAT filesystem initialized\n", TC_LRED);
        return 1;
    }

    const char* path = (argc == 2) ? argv[1] : currentDir;

    if (strcmp(path, "root") == 0 || strcmp(path, "/") == 0) {
        path = "";
    }

        //shitty implementation to get rid of FAT_OpenAbsolute not being able to find root/BOOT for example
    if(starts_with(path,"root/")) {
        path += 5;
    }

    FAT_file_t* file = FAT_OpenAbsolute(s_fat_fs, path);
    if (file == NULL) {
        term_write("ERROR: Not found: '", TC_LRED);
        term_write(path, TC_LRED);
        term_write("'\n", TC_LRED);
        return 1;
    }

    char** names = NULL;
    size_t count = FAT_ListFiles(file, &names);
    if (names == NULL) {
        term_write("ERROR: Failed to list files\n", TC_LRED);
        FAT_Close(file);
        return 1;
    }

    term_write("Name                Type\n", TC_LBLUE);
    for (size_t i = 0; i < count; i++) {
        term_write(names[i], TC_WHITE);

        for (size_t j = 0; j < 20 - strlen(names[i]); j++)
        {
            term_putchar(' ', TC_WHITE);
        }

        FAT_file_t* file = FAT_OpenAbsolute(s_fat_fs, names[i]);
        
        if(FAT_IsDirectory(file)) {
            term_write("DIR", TC_WHITE);
        } else {
            term_write("FILE", TC_WHITE);
        }
        kfree(file);
        term_putchar('\n', TC_WHITE);
        kfree(names[i]);
    }
    term_putchar('\n', TC_WHITE);
    kfree(names);

    FAT_Close(file);

    return 0;
}