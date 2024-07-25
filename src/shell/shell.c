/* The Choacury CLI Shell */

#include "../drivers/filesystem/fat.h"
#include "../drivers/pit.h"
#include "../drivers/ps2_keyboard.h"
#include "../drivers/sound.h"
#include "../drivers/storage/device.h"
#include "../drivers/utils.h"
#include "../drivers/vga.h"
#include "../memory/kmalloc.h"
#include "shell.h"
#include "terminal.h"

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGUMENTS 128

static FAT_filesystem_t* s_fat_fs = NULL;
char currentDir[] = "root";                 // <-- The current directory for stuff like 'CD' (WIP)

static void handle_command(int argc, const char** argv) {
    if (argc == 0) {
        return;
    }
	
	if(strcmp(argv[0], "guiload") == 0){
		vga_init(1);    // <- Starts the GUI (VGA Mode 13)
	}

    if (strcmp(argv[0], "hello") == 0) {
        /* Basic testing command */
        term_write("Hello from Terminal\n", TC_WHITE);
    }

    else if (strcmp(argv[0], "help") == 0) {
        /* actual help information, might need to be rewriten in the future */
        if (strcmp(argv[1], "calc") == 0) {
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
        else if (strcmp(argv[1], "beep") == 0) {
            term_write("BEEP\n\n", TC_WHITE);
            term_write("Beeps. Syntax: ", TC_WHITE);
            term_write("beep FREQ. DUR.\n\n", TC_BRIGHT);
            term_write("NOTE! This requires PC Speaker/Beeper support on your computer\n", TC_WHITE);
        }
		else if (strcmp(argv[1], "vgainit") == 0) {
            term_write("VGAINIT\n\n", TC_WHITE);
            term_write("Initializes the VGA 0x13 mode. Supports drawing simple shapes.", TC_WHITE);
        }
        /* if no command is present in arg 1 */
        else {
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
        int MathFunction = 0;

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

        // a shitty way of making a barebones calculator but it works //
        if (strcmp(argv[2], "+") == 0 || strcmp(argv[2], "-a")  == 0) {
            MathFunction = 1;
        }
        else if (strcmp(argv[2], "-") == 0 || strcmp(argv[2], "-s") == 0) {
            MathFunction = 2;
        }
        else if (strcmp(argv[2], "/") == 0 || strcmp(argv[2], "-d") == 0) {
            MathFunction = 3;
        }
        else if (strcmp(argv[2], "*") == 0 || strcmp(argv[2], "-m")) {
            MathFunction = 4;
        }
        else {
            term_write("ERROR: Not a vaild function: ", TC_LRED);
            term_write(argv[2], TC_BRIGHT);
            term_write("Confused? Use ", TC_WHITE);
            term_write("HELP CALC", TC_BRIGHT);
            term_write(" for command information\n", TC_WHITE);
            return;
        }

        /* Basic mathimatical functions*/
        if (MathFunction == 1){
            term_write_u32(number1.value + number2.value, 10, TC_WHITE);
        }
        else if (MathFunction == 2){
            term_write_u32(number1.value - number2.value, 10, TC_WHITE);
        }
        else if (MathFunction == 3){
            /* prevent dividing by 0 */
            if (number1.value == 0 || number2.value == 0) {
                term_write("Can not divide by 0!\n", TC_LRED);
                return;
            }
            else {term_write_u32(number1.value / number2.value, 10, TC_WHITE);}
        }
        else if (MathFunction == 4) {
            term_write_u32(number1.value * number2.value, 10, TC_WHITE);
        }
        term_write("\n", TC_WHITE);
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
        term_write("BUILD: ", TC_LBLUE);
        term_write(__DATE__ " @ " __TIME__ "\n", TC_WHITE);
        term_write("KERNEL: ", TC_LBLUE);
        term_write("Choacury Standard (FS Testing)\n", TC_WHITE);
        term_write("SHELL: ", TC_LBLUE);
        term_write("chsh-0.0.0.0039a-dev\n", TC_WHITE);       // <-- Could be improved on
        term_write("RAM: ", TC_LBLUE);
        term_write("RAM Counter goes here" "\n", TC_WHITE); // <-- Obviously a placeholder
        term_write("CPU: ", TC_LBLUE);
        term_write("CPU Info code goes here" "\n", TC_WHITE);
    }

    else if (strcmp(argv[0], "cd") == 0) {
        if(argc != 2){
            term_write("ERROR: Usage -> cd PATH\n", TC_LRED);
            return;
        }
        if (s_fat_fs == NULL) {
            term_write("ERROR: Not FAT filesystem initialized\n", TC_LRED);
            return;
        }

        // TOADD: The actual CD stuff
        /* PLACEHOLDER DECECTION*/
        term_write("WARNING: CD command is still being developed, so that ", TC_YELLO);
        term_write(argv[1], TC_BRIGHT);
        term_write(" may not be a vaild directory and may cause issues\n", TC_YELLO);
        // char currentDir[] = argv[1]; // <-- Sets the current directory to Arg1, making it a proper change DIR command

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

		const char* path = (argc == 2) ? argv[1] : "";

        FAT_file_t* file = FAT_OpenAbsolute(s_fat_fs, path);
		if (file == NULL) {
			term_write("ERROR: Not found: '", TC_LRED);
			term_write(argv[1], TC_LRED);
			term_write("'\n", TC_LRED);
			FAT_Close(file);
			return;
		}

		char** names = NULL;
		size_t count = FAT_ListFiles(file, &names);
		if (names == NULL) {
			term_write("EHH??", TC_YELLO);
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
            case KEY_LeftCtrl:      // TODO: <-- Replace with Ctrl+G (Bell command on most other systems)
                startbeep(800);
                pit_sleep_ms(15);
                mutebeep();
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
