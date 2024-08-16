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

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGUMENTS 128

static FAT_filesystem_t* s_fat_fs = NULL;
char currentDir[256] = "root";
/* Math dictionary*/

/* Define math functions */
typedef int (*math_op_t)(int, int);
int add(int a, int b);
int subtract(int a, int b);
int divide(int a, int b);
int multiply(int a, int b);
typedef struct {
    const char *op;
    math_op_t func;
} op_map_t;

op_map_t operations[] = {
    {"+", add},
    {"-a", add},
    {"-", subtract},
    {"-s", subtract},
    {"/", divide},
    {"-d", divide},
    {"*", multiply},
    {"-m", multiply},
    {NULL, NULL} // <- End of map
};
int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int divide(int a, int b) {
    return a / b;
}

int multiply(int a, int b) {
    return a * b;
}
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

    /* Get CPU vendor string */
    cpuid(0, &eax, &ebx, &ecx, &edx);
    ((uint32_t*)vendor)[0] = ebx;
    ((uint32_t*)vendor)[1] = edx;
    ((uint32_t*)vendor)[2] = ecx;
    vendor[12] = '\0';

    /* Get CPU brand string (if supported) */
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

static void handle_command(int argc, const char** argv) {
    if (argc == 0) {
        return;
    }

    if(strcmp(argv[0], "guiload") == 0){
        /* Initialize graphics mode and start desktop */
	    vga_graphics_init(TC_BLUE);
        start_desktop();

        /* If desktop exits, reinitialize text mode and render terminal */
	    vga_text_init(TC_BLACK);
        term_rerender_buffer();
    }
	
	else if(strcmp(argv[0], "vbetest")==0){
		BgaSetVideoMode(FHD,VBE_DISPI_BPP_32,1,1);
		//vbe_putpixel(0,0,0x000000ff);

	}

    else if (strcmp(argv[0], "hello") == 0) {
        /* Basic testing command */
        term_write("Hello from Terminal\n", TC_WHITE);
    }

    /* actual help information, might need to be rewriten in the future */
    else if (strcmp(argv[0], "help") == 0) {
        /* if no command is present in arg 1 */
        if (argc == 1) {
            // TOADD:
            // - Proper file and directory creation and deletion commands, like MKDIR, MF, etc.
            term_write("LIST OF COMMANDS\n", TC_WHITE);
            term_write("help                - Hello there! I'm the Help Command!\n", TC_WHITE);
            term_write("beep                - PC Beeper control. \n", TC_WHITE);
            term_write("calc                - Literally a Calculator\n", TC_WHITE);
            term_write("cat                 - Print file contents.\n", TC_WHITE);
            term_write("cd                  - Changes the current directory\n", TC_WHITE);
            term_write("compdate            - Shows the compilation date.\n", TC_WHITE);
            term_write("cls OR clear        - Clears the screen.\n", TC_WHITE);
            term_write("echo                - Prints string to the console.\n", TC_WHITE);
            term_write("guiload             - Loads up the GUI (WIP!)\n", TC_WHITE);
            term_write("ls                  - List files in a directory.\n", TC_WHITE);
            term_write("pause               - Pauses the terminal until a keyboard input.\n", TC_WHITE);
            term_write("pl                  - How many data devices are detected.\n", TC_WHITE);
            term_write("chstat              - Display system information.\n", TC_WHITE);
			term_write("vbetest             - Test Bochs VBE extensions.\n", TC_WHITE);
        }
        else if (strcmp(argv[1], "calc") == 0) {
            term_write("CALC\n\n", TC_WHITE);
            term_write("Calculate math. Syntax: ", TC_WHITE);
            term_write("calc NUMBER1 FUNCT NUMBER2\n\n", TC_BRIGHT);
            term_write("+ or -a             - Add two numbers together\n", TC_WHITE);
            term_write("- or -s             - Subtract two numbers together\n", TC_WHITE);
            term_write("* or -m             - Multiply two numbers together\n", TC_WHITE);
            term_write("/ or -d             - Divide two numbers together\n", TC_WHITE);
        }
        else if (strcmp(argv[1], "cat") == 0) {
            term_write("CAT\n\n", TC_WHITE);
            term_write("Shows files contants. Syntax: ", TC_WHITE);
            term_write("cat PATH/TO/FILE\n\n", TC_BRIGHT);
        }
        else if (strcmp(argv[1], "ls") == 0) {
            term_write("LS\n\n", TC_WHITE);
            term_write("Displays files and folder in a directory. Syntax: ", TC_WHITE);
            term_write("ls DIRECTORY\n\n", TC_BRIGHT);
            term_write("NOTE! This requires FAT to be init properly.\n", TC_WHITE);
        }
        else if (strcmp(argv[1], "beep") == 0) {
            term_write("BEEP\n\n", TC_WHITE);
            term_write("Beeps. Syntax: ", TC_WHITE);
            term_write("beep FREQ. DUR.\n\n", TC_BRIGHT);
            term_write("NOTE! This requires PC Speaker/Beeper support on your computer\n", TC_WHITE);
        }
        else if (strcmp(argv[1], "guiload") == 0) {
            term_write("GUILOAD\n\n", TC_WHITE);
            term_write("Initializes the VGA 0x13 mode. Supports drawing simple shapes.\n", TC_WHITE);
            term_write("In short it boots up a GUI, which supports 256 colours.\n", TC_WHITE);
        }
        else if (strcmp(argv[1], "chstat") == 0) {
            term_write("CHSTAT\n\n", TC_WHITE);
            term_write("Displays system info. Such as RAM, CPU, OS Information, etc.\n", TC_WHITE);
        }
        else {
            term_write(argv[1], TC_BRIGHT);
            term_write(" is not a valid command, so no help information will be displayed\n", TC_WHITE);
        }
    }

    else if (strcmp(argv[0], "echo") == 0) {
        for (int i = 1; i < argc; i++) {
            if (i > 1)
                term_write(" ", TC_WHITE);
            term_write(argv[i], TC_WHITE);
        }
        term_write("\n", TC_WHITE);
    }

    else if (strcmp(argv[0], "beep") == 0) {
        if (argc != 2 && argc != 3) {
            term_write("ERROR: Usage -> beep [freq.] [duration]\n", TC_LRED);
            return;
        }

        atoi_result_t frequency = atoi(argv[1]);
        if (!frequency.valid) {
            term_write("ERROR: Frequency provided is not an interger\n", TC_LRED);
            return;
        }

        /* Use the default beep length if no value has been set by the user */
        atoi_result_t duration = { .valid = true, .value = 500 };    // <-- 500 ms
        if (argc >= 3) {
            duration = atoi(argv[2]);
        }
        if (!duration.valid) {
            term_write("ERROR: Duration provided is not an interger\n", TC_LRED);
            return;
        }

        startbeep(frequency.value);
        pit_sleep_ms(duration.value);
        mutebeep();
    }

    else if (strcmp(argv[0], "calc") == 0) {

        if (argc != 4) {
            term_write("ERROR: Usage -> calc [number1] [func] [number2]\n", TC_LRED);
            term_write("Confused? Use ", TC_WHITE);
            term_write("HELP CALC", TC_BRIGHT);
            term_write(" for command information\n", TC_WHITE);
            return;
        }

        atoi_result_t number1 = atoi(argv[1]);
        if (!number1.valid) {
            term_write("ERROR: First number provided is not an interger\n", TC_LRED);
            return;
        }

        atoi_result_t number2 = atoi(argv[3]);
        if (!number2.valid) {
            term_write("ERROR: First number provided is not an interger\n", TC_LRED);
            return;
        }

        math_op_t op_func = NULL;
        for (int i = 0; operations[i].op != NULL; i++) {
            if (strcmp(argv[2], operations[i].op) == 0) {
                op_func = operations[i].func;
                break;
            }
        }

        if (op_func == NULL) {
            term_write("ERROR: Not a valid function: ", TC_LRED);
            term_write(argv[2], TC_BRIGHT);
            term_write("Confused? Use ", TC_WHITE);
            term_write("HELP CALC", TC_BRIGHT);
            term_write(" for command information\n", TC_WHITE);
            return;
        }

        // Perform the calculation and handle division by zero
        int result = op_func(number1.value, number2.value);
        if (op_func == divide && number2.value == 0) {
            term_write("ERROR: Cannot divide by 0!\n", TC_LRED);
        } else {
            term_write_u32(result, 10, TC_WHITE);
            term_write("\n", TC_WHITE);
        }
    }


    else if (strcmp(argv[0], "cls") == 0 || strcmp(argv[0], "clear") == 0) {
        term_clear();
    }

    else if (strcmp(argv[0], "pause") == 0) {
        if(strcmp(argv[1], "-t") == 0) {
            atoi_result_t duration = { .valid = true, .value = 500 };    // <-- 500 ms
            if (argc >= 3) {
                duration = atoi(argv[2]);
            }
            if (!duration.valid) {
                term_write("ERROR: Duration provided is not an interger\n", TC_LRED);
                return;
            }
            pit_sleep_ms(duration.value);
        }
        else if(strcmp(argv[1], "-k") == 0) {
            term_write("Press any key to continue.\n", TC_WHITE);
            asm("hlt");
        }
        else{
            term_write("ERROR: Invaild option/choice.\n", TC_LRED);
        }
    }

    /* Show every detected storage device and their partitions */
    else if (strcmp(argv[0], "pl") == 0) {
        term_write("Detected Devices: ", TC_WHITE);
        term_write_u32(g_storage_device_count, 10, TC_WHITE);
        term_write("\n", TC_WHITE);
        for (int i = 0; i < g_storage_device_count; i++) {
            storage_device_t* device = g_storage_devices[i];
            u64 device_size = device->sector_count * device->sector_size;

            term_write("  ", TC_WHITE);
            term_write(device->model, TC_WHITE);
            term_write(" (", TC_WHITE);
            term_write_u32(device_size / 1024 / 1024, 10, TC_WHITE);
            term_write(" MiB)\n", TC_WHITE);

            for (u32 j = 0; j < device->partition_count; j++) {
                storage_device_t* partition = device->partitions[j];
                u64 partition_size = partition->sector_count * partition->sector_size;

                term_write("  \xC0 ", TC_WHITE);        // <- Stylistic choice.
                                                        // TOFIX: Use Hex C4 (├) for any other partition
                                                        // and save Hex C0 (└) for that last partition in each list
                term_write(partition->model, TC_WHITE);
                term_write(" (", TC_WHITE);
                term_write_u32(partition_size / 1024 / 1024, 10, TC_WHITE);
                term_write(" MiB)\n", TC_WHITE);
            }
        }
    }

    else if (strcmp(argv[0], "chstat") == 0) {
        /* this is basically a stupid neofetch clone */
        char mem_mib_buffer[20];
        uint64_to_string(g_total_pmm_bytes / 1024 / 1024, mem_mib_buffer);
        mem_mib_buffer[19] = 0;                                 // <-- to prevent undefined behaviour
        char cpu_vendor[13];
        char cpu_brand[49];


        get_cpu_info(cpu_vendor, cpu_brand);

        term_write("BUILD: ", TC_LBLUE);
        term_write(__DATE__ " @ " __TIME__ "\n", TC_WHITE);
        term_write("KERNEL: ", TC_LBLUE);
        term_write("Choacury Standard\n", TC_WHITE);            // <-- aka. stock kernel.
        term_write("SHELL: ", TC_LBLUE);
        term_write("chsh-0.0.0.0041e-dev\n", TC_WHITE);         // <-- Could be more automated ngl.
        term_write("RAM: ", TC_LBLUE);
        term_write(mem_mib_buffer, TC_WHITE);
        term_write(" MiB\n", TC_WHITE);
        term_write("CPU Vendor: ", TC_LBLUE);
        term_write(cpu_vendor, TC_WHITE);
        term_write("\n", TC_WHITE);

        term_write("CPU Brand: ", TC_LBLUE);
        term_write(cpu_brand[0] ? cpu_brand : "N/A", TC_WHITE);  // If brand is empty, show "N/A"
        term_write("\n", TC_WHITE);
    }

    else if (strcmp(argv[0], "cd") == 0) {
    if (argc != 2) {
        term_write("ERROR: Usage -> cd PATH\n", TC_LRED);
        return;
    }

    if (s_fat_fs == NULL) {
        term_write("ERROR: FAT filesystem not initialized\n", TC_LRED);
        return;
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
        return;
    }

    if (!FAT_IsDirectory(new_dir)) {
        term_write("ERROR: Not a directory: '", TC_LRED);
        term_write(full_path, TC_LRED);
        term_write("'\n", TC_LRED);
        FAT_Close(new_dir);
        return;
    }

    // Update the current dir
    strcpy(currentDir, full_path);

    term_write(currentDir, TC_WHITE);
    term_write("\n", TC_WHITE);

    FAT_Close(new_dir);
}



    else if (strcmp(argv[0], "mf") == 0) {
        /* this pretty much is meant to make a blank file */
        term_write("MF is still yet to be added\n", TC_WHITE);    // <-- placeholder

    }

    else if (strcmp(argv[0], "cat") == 0) {
        if (argc != 2) {
            term_write("ERROR: Usage -> cat PATH\n", TC_LRED);
            return;
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
    }

    else if (strcmp(argv[0], "ls") == 0) {
        if (argc > 2) {
            term_write("ERROR: Usage -> ls [PATH]\n", TC_LRED);
            return;
        }
        if (s_fat_fs == NULL) {
            term_write("ERROR: Not FAT filesystem initialized\n", TC_LRED);
            return;
        }

        const char* path = (argc == 2) ? argv[1] : currentDir;

        if (strcmp(path, "root") == 0 || strcmp(path, "/") == 0) {
            path = "";
        }

        //shitty implementation to get rid of FAT_OpenAbsolute not being able to find root/BOOT for example
        if(starts_with(path,"root/")){
        path+=5;
        }
        FAT_file_t* file = FAT_OpenAbsolute(s_fat_fs, path);
        if (file == NULL) {
            term_write("ERROR: Not found: '", TC_LRED);
            term_write(path, TC_LRED);
            term_write("'\n", TC_LRED);
            return;
        }

        char** names = NULL;
        size_t count = FAT_ListFiles(file, &names);
        if (names == NULL) {
            term_write("ERROR: Failed to list files\n", TC_LRED);
            FAT_Close(file);
            return;
        }

        for (size_t i = 0; i < count; i++) {
            term_write(names[i], TC_WHITE);
            term_putchar(' ', TC_WHITE);
            kfree(names[i]);
        }
        term_putchar('\n', TC_WHITE);
        kfree(names);

        FAT_Close(file);
    }



    else if (strcmp(argv[0], "compdate") == 0) {
        term_write(__DATE__ "\n", TC_WHITE);
    }

    else if (strcmp(argv[0], "whereami") == 0) {
        term_write(currentDir, TC_WHITE);
        term_write("\n", TC_WHITE);
    }

    else {
        term_write(argv[0], TC_YELLO);
        term_write(" is not a valid command, file, or program.\n", TC_YELLO);
    }
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

/* Main CLI shell stuff. */
void shell_start() {

    // FIXME: This should be done in vfs.
    s_fat_fs = FAT_Init(g_storage_devices[0]->partitions[1]);
    if (s_fat_fs == NULL) {
        term_write("Could not initialize FAT from storage_device[0], partition[1]\n", TC_YELLO);
    }
    else {
        term_write("Initialized FAT!\n", TC_WHITE);
    }

    // TOADD: Ctrl Command Codes (i.e. Ctrl+C to close a program, etc.)
    char command_buffer[MAX_COMMAND_LENGTH];
    unsigned command_length = 0;

    term_write(currentDir, TC_LIME);
    term_write("> ", TC_WHITE);

    for (;;) {
        key_event_t event;
        ps2_get_key_event(&event);

        /* Halt the CPU until an interrupt if there is no input/ */
        if (event.key == KEY_NONE) {
            asm volatile("hlt");
            continue;
        }

        /* Discard key release events. */
        if (event.modifiers & KEY_EVENT_MODIFIERS_RELEASED) {
            continue;
        }

        /* we should add support for stuff like Ctrl+Alt+Del */

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
                    parse_command(command_buffer, command_length);
                    command_length = 0;
                }
                term_write(currentDir, TC_LIME);
                term_write("> ", TC_WHITE);
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
