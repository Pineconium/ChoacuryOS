#include "../drivers/pit.h"
#include "../drivers/ps2_keyboard.h"
#include "../drivers/sound.h"
#include "../drivers/utils.h"
#include "../drivers/vga.h"
#include "../drivers/storage/device.h"
#include "shell.h"
#include "terminal.h"

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGUMENTS 128

static void handle_command(int argc, const char** argv) {
    if (argc == 0) {
        return;
    }

    if (strcmp(argv[0], "hello") == 0) {
        // Standard Testing command
        term_write("Hello from Terminal\n", TC_WHITE);
    }
    else if (strcmp(argv[0], "help") == 0) {
        term_write("LIST OF COMMANDS\n", TC_WHITE);
        term_write("help                - Hello there! I'm the Help Command!\n", TC_WHITE);
        term_write("beep (FREQ)(timems) - PC Beeper control. \n", TC_WHITE);
        term_write("compdate            - Shows the compilation date.\n", TC_WHITE);
        term_write("cls                 - Clears the screen.\n", TC_WHITE);
        term_write("echo (string)       - Prints string to the console\n", TC_WHITE);
        term_write("pl                  - How many data devices are detected.\n", TC_WHITE);
        // term_write("pchar (HEX)(COLOUR) - Prints a CP437 char. to the console\n", TC_WHITE); <-- Planned addition
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
            term_write("ERROR: Usage: beep [freq.] [duration]\n", TC_LRED);
            return;
        }

        atoi_result_t frequency = atoi(argv[1]);
        if (!frequency.valid) {
            term_write("ERROR: Frequency is not an interger\n", TC_LRED);
            return;
        }

        // default beep length (500 ms)
        atoi_result_t duration = { .valid = true, .value = 500 };
        if (argc >= 3) {
            duration = atoi(argv[2]);
        }
        if (!duration.valid) {
            term_write("ERROR: Duration is not an interger\n", TC_LRED);
            return;
        }

        startbeep(frequency.value);
        pit_sleep_ms(duration.value);
        mutebeep();
    }
    else if (strcmp(argv[0], "cls") == 0) {
        term_clear();
    }

    /* Placeholder File system access func. */
    else if (strcmp(argv[0], "pl") == 0) {
        term_write("Storage Devices: ", TC_WHITE);
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

                term_write("    ", TC_WHITE);
                term_write(partition->model, TC_WHITE);
                term_write(" (", TC_WHITE);
                term_write_u32(partition_size / 1024 / 1024, 10, TC_WHITE);
                term_write(" MiB)\n", TC_WHITE);
            }
        }
    }
    else if (strcmp(argv[0], "compdate") == 0) {
        term_write(__DATE__ "\n", TC_WHITE);
    }
    else {
        term_write("ERROR: Unknown command: ", TC_YELLO);
        term_write(argv[0], TC_YELLO);
        term_write("\n", TC_YELLO);
    }
}

// parse command buffer to null-terminated list of arguments
static void parse_command(char* command, unsigned length) {
    // make sure command buffer is null-terminated
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
    char command_buffer[MAX_COMMAND_LENGTH];
    unsigned command_length = 0;

    term_write("> ", TC_WHITE);

    for (;;) {
        key_event_t event;
        ps2_get_key_event(&event);

        /* Halt the CPU until an interrupt if theres no input */
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
                    parse_command(command_buffer, command_length);
                    command_length = 0;
                }
                term_write("> ", TC_WHITE);
                break;
            case KEY_LeftCtrl:      // <-- Replace with Ctrl+G (Bell command on most other systems)
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
