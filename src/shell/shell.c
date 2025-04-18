/* The Choacury CLI Shell */

#include "../drivers/debug.h"
#include "../drivers/filesystem/fat.h"
#include "../drivers/pit.h"
#include "../drivers/ps2_keyboard.h"
#include "../drivers/sound.h"
#include "../drivers/storage/device.h"
#include "../drivers/utils.h"
#include "../drivers/vga.h"
#include "../drivers/vbe.h"
#include "../gui/desktop.h"
#include "../kernel/panic.h"
#include "../memory/kmalloc.h"
#include "../memory/pmm.h"
#include "shell.h"
#include "terminal.h"
#include <stdint.h>

// Commands include
#include "commands/command.h"

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGUMENTS 128

FAT_filesystem_t* s_fat_fs = NULL;
char currentDir[256] = "root";
/* Math dictionary*/


// Define function prototypes for math operations

typedef int (*math_op_t)(int, int);
int add(int a, int b);
int subtract(int a, int b);
int divide(int a, int b);
int multiply(int a, int b);

/* find the last slash */
char* find_last_slash(char* str) {
    char* last_slash = NULL;
    while (*str) {
        if (*str == '/') {
            last_slash = str;
        }
        str++;
    }
    return last_slash;
}
int starts_with(const char *str, const char *prefix) {
    while (*prefix) {
        if (*str != *prefix) {
            return 0;   // <- Characters do not match
        }
        str++;
        prefix++;
    }
    return 1;           // <- All characters matched
}
void cpuid(uint32_t eax_in, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
    __asm__ volatile (
        "cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "a" (eax_in)
    );
}

void get_cpu_info(char* vendor, char* brand) {
    uint32_t eax, ebx, ecx, edx;

    /* fetch the cpu vendor */
    cpuid(0, &eax, &ebx, &ecx, &edx);
    ((uint32_t*)vendor)[0] = ebx;
    ((uint32_t*)vendor)[1] = edx;
    ((uint32_t*)vendor)[2] = ecx;
    vendor[12] = '\0';

    /* fetch the cpu brand, if supported */
    cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
    if (eax >= 0x80000004) {
        uint32_t* brand_ptr = (uint32_t*)brand;
        cpuid(0x80000002, &brand_ptr[0], &brand_ptr[1], &brand_ptr[2], &brand_ptr[3]);
        cpuid(0x80000003, &brand_ptr[4], &brand_ptr[5], &brand_ptr[6], &brand_ptr[7]);
        cpuid(0x80000004, &brand_ptr[8], &brand_ptr[9], &brand_ptr[10], &brand_ptr[11]);
        brand[48] = '\0';
    } else {
        brand[0] = '\0';
    }
}

static void handle_return_code(int return_code, int argc, char** argv) {
    // Look in commands/structure.md for more information //

    if(return_code == 2) {
        // 2 = Incorrect arguments return code
        // Display the help page for the specific commands (Flagged with the argument flag to be more specific)
        term_write("Invalid arguments.\n", TC_YELLO);
        shell_commands_list[0].func(3, (char*[]){ "help", argv[0], "args" });
    }
}

static void handle_command(int argc, const char** argv) {
    if(argc == 0) {
        return; // <- No point in doing anything if they haven't entered a command
    }

    for (size_t i = 0; i < shell_commands_count; i++)
    {
        if(strcmp(shell_commands_list[i].name, argv[0]) == 0) {
            /* Found command */

            handle_return_code(shell_commands_list[i].func(argc, argv), argc, argv);

            return;
        }

        for (size_t j = 0; shell_commands_list[i].aliases[j] != NULL; j++)
        {
            if(strcmp(shell_commands_list[i].aliases[j], argv[0]) == 0) {
                /* found command */

                handle_return_code(shell_commands_list[i].func(argc, argv), argc, argv);

                return;
            }
        }
    }

    term_write(argv[0], TC_YELLO);
    term_write(" is not a valid command, file, or program.\n", TC_YELLO);
}

/* Parse command buffer to a null-terminated list of arguments */
static void parse_command(char* command, unsigned length) {
    /* Make sure that the command buffer is null-terminated */
    command[length] = 0;

    const char* arguments[MAX_ARGUMENTS];
    int argument_count = 0;

    unsigned start = 0;
    for (unsigned i = 0; i < length; i++) {
        if (command[i] != ' ') {
            continue;
        }

        if (i != start && argument_count < MAX_ARGUMENTS) {
            arguments[argument_count] = &command[start];
            argument_count++;
        }
        command[i] = 0;
        start = i + 1;
    }

    if (start < length && argument_count < MAX_ARGUMENTS) {
        arguments[argument_count] = &command[start];
        argument_count++;
    }

    handle_command(argument_count, arguments);
}

// PSF1

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04
/*
typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

typedef struct {
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
} PSF1_FONT;
*/

PSF1_FONT* font = NULL;

PSF1_FONT* LoadFont() {
    // Open the font file
    FAT_file_t* font = FAT_OpenAbsolute(s_fat_fs, "/Unifont.psf");
    if (font == NULL) {
		term_write("FAT_ERR: CANNOT OPEN PSF1 FONT FILE! (1/1)\n", TC_LRED);
        return NULL; // Handle error if file can't be opened
    }

    // Allocate memory for the PSF1_FONT structure
    PSF1_FONT* loadedFont = (PSF1_FONT*)kmalloc(sizeof(PSF1_FONT));
    if (loadedFont == NULL) {
        FAT_Close(font);
		term_write("MALLOC_ERR: MEM ALLOCATION FAIL! (1/3)\n", TC_LRED);
        return NULL; // Handle memory allocation failure
    }

    // Allocate memory for the PSF1 header and read it from the file
    loadedFont->psf1_Header = (PSF1_HEADER*)kmalloc(sizeof(PSF1_HEADER));
    if (loadedFont->psf1_Header == NULL) {
        kfree(loadedFont);
        FAT_Close(font);
		term_write("MALLOC_ERR: MEM ALLOCATION FAIL! (2/3)\n", TC_LRED);
        return NULL; // Handle memory allocation failure
    }

    // Read the PSF1 header
    size_t bytesRead = FAT_Read(font, 0, loadedFont->psf1_Header, sizeof(PSF1_HEADER));
    if (bytesRead != sizeof(PSF1_HEADER)) {
        kfree(loadedFont->psf1_Header);
        kfree(loadedFont);
        FAT_Close(font);
		term_write("PSF1_ERR: READ FAIL! (1/2)\n", TC_LRED);
        return NULL; // Handle read failure
    }

    // Check if the file is a valid PSF1 font by verifying the magic number
    if (loadedFont->psf1_Header->magic[0] != PSF1_MAGIC0 || loadedFont->psf1_Header->magic[1] != PSF1_MAGIC1) {
        kfree(loadedFont->psf1_Header);
        kfree(loadedFont);
        FAT_Close(font);
		term_write("PSF1_ERR: INVALID FONT! (1/1)\n", TC_LRED);
        return NULL; // Handle invalid PSF1 font
    }

    // Calculate the size of the glyph buffer and allocate memory
    size_t glyphBufferSize = loadedFont->psf1_Header->charsize * 256; // Standard PSF1 fonts have 256 glyphs
    if (loadedFont->psf1_Header->mode == 1) {
        glyphBufferSize *= 2; // PSF1 fonts with mode 1 have 512 glyphs
    }

    loadedFont->glyphBuffer = kmalloc(glyphBufferSize);
    if (loadedFont->glyphBuffer == NULL) {
        kfree(loadedFont->psf1_Header);
        kfree(loadedFont);
        FAT_Close(font);
		term_write("MALLOC_ERR: MEM ALLOCATION FAIL! (3/3)\n", TC_LRED);
        return NULL; // Handle memory allocation failure
    }

    // Read the glyph data into the buffer
    bytesRead = FAT_Read(font, 0, loadedFont->glyphBuffer, glyphBufferSize);
    if (bytesRead != glyphBufferSize) {
        kfree(loadedFont->glyphBuffer);
        kfree(loadedFont->psf1_Header);
        kfree(loadedFont);
        FAT_Close(font);
		term_write("PSF1_ERR: READ FAIL! (2/2)\n", TC_LRED);
        return NULL; // Handle read failure
    }

    // Close the font file
    FAT_Close(font);

    // Return the loaded font structure
    return loadedFont;
}

/* Main CLI shell stuff. */
void shell_start() {
    // Existing initialization code for the shell
    s_fat_fs = FAT_Init(g_storage_devices[0]->partitions[1]);
    if (s_fat_fs == NULL) {
        term_write("Could not initialize FAT from storage_device[0], partition[1]\n", TC_YELLO);
    } else {
        term_write("Initialized FAT!\n", TC_WHITE);
    }

    char command_buffer[MAX_COMMAND_LENGTH];
    unsigned command_length = 0;

    term_write("Initialising commands list... ", TC_WHITE);
    shell_init_commands_list();
    term_write("Done!\n", TC_GREEN);
	
	term_write("Creating test dir...\n", TC_GREEN);
	FAT_file_t* parent_dir = FAT_OpenAbsolute(s_fat_fs, "/");
	FAT_CreateDirectory(parent_dir,"/TEST0");
	
	term_write("Creating test file...\n", TC_GREEN);
	
	char teststr = "Hello, World!";
	FAT_file_t* file = FAT_OpenAbsolute(s_fat_fs, "/test.txt");
	FAT_Write(file, 0, teststr, strlen(teststr));
	FAT_Close(file);

    term_write("Loading font... ", TC_WHITE);
    font = LoadFont();
    term_write("Done!\n", TC_GREEN);

    term_write(currentDir, TC_LIME);
    term_write("> ", TC_WHITE);
	


    for (;;) {
        key_event_t event;
        ps2_get_key_event(&event);

        /* Halt the CPU until an interrupt if there is no input */
        if (event.key == KEY_NONE) {
            asm volatile("hlt");
            continue;
        }

        /* Discard key release events */
        if (event.modifiers & KEY_EVENT_MODIFIERS_RELEASED) {
            continue;
        }

        switch (event.key) {
            case KEY_Backspace:
                if (command_length > 0) {
                    term_write("\b \b", TC_WHITE);
                    command_length--;
                }
                break;
            case KEY_Enter:
                term_putchar('\n', TC_WHITE);
                if (command_length > 0) {
                    command_buffer[command_length] = '\0';
                    add_command_to_history(command_buffer);

                    parse_command(command_buffer, command_length);
                    command_length = 0;
                }

                // After processing the command, check if we need to scroll
                if (s_term_info.row >= s_term_info.height) {
                    term_scroll();
                    s_term_info.row = s_term_info.height - 1;
                }

                term_write(currentDir, TC_LIME);
                term_write("> ", TC_WHITE);
                break;
            case KEY_ArrowDown:
                if (cmd_history.history_count > 0) {
                    if (cmd_history.history_position >= 0) {
                        cmd_history.history_position++;
                        if (cmd_history.history_position >= cmd_history.history_count) {
                            // If we exceed the history, reset to a blank command line
                            cmd_history.history_position = -1;
                        }

                        // Clear the current input
                        while (command_length > 0) {
                            term_write("\b \b", TC_WHITE);
                            command_length--;
                        }

                        // If we're not at the blank command line, copy the history command to the buffer
                        if (cmd_history.history_position >= 0) {
                            strcpy(command_buffer, cmd_history.commands[cmd_history.history_position]);
                            command_length = strlen(command_buffer);
                            term_write(command_buffer, TC_WHITE);
                        }
                    }
                }
                break;
            case KEY_ArrowUp:
                if (cmd_history.history_count > 0) {
                    if (cmd_history.history_position < 0) {
                        cmd_history.history_position = cmd_history.current_index - 1;
                    } else {
                        cmd_history.history_position--;
                        if (cmd_history.history_position < 0) {
                            cmd_history.history_position = cmd_history.history_count - 1;
                        }
                    }

                    // Clear the current input
                    while (command_length > 0) {
                        term_write("\b \b", TC_WHITE);
                        command_length--;
                    }

                    // Copy the history command to the buffer
                    strcpy(command_buffer, cmd_history.commands[cmd_history.history_position]);
                    command_length = strlen(command_buffer);
                    term_write(command_buffer, TC_WHITE);
                }
                break;
            case KEY_Tab:
                if (command_length+4>=sizeof(command_buffer)){
                    //don't try to add the tab if the buffer doesn't have enough space
                    break;
                }
                if (command_length > 0){
                    for (int i=0;i<4;++i){
                        term_write(" ",TC_WHITE);
                        command_buffer[command_length] = ' ';
                        command_length++;

                    }
                }
                break;
            default: {
                const char* utf8 = key_to_utf8(&event);
                while (utf8 && *utf8) {
                    if (command_length >= sizeof(command_buffer) - 1) {
                        break;
                    }
                    term_putchar(*utf8, TC_WHITE);
                    command_buffer[command_length] = *utf8;
                    command_length++;
                    utf8++;
                }
                break;
            }
        }
    }
}
