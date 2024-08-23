#include "pause.h"

#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"

int shell_pause_command(int argc, const char** argv) {
    if(strcmp(argv[1], "-t") == 0) {
        atoi_result_t duration = { .valid = true, .value = 500 };    // <-- 500 ms
        if (argc >= 3) {
            duration = atoi(argv[2]);
        }
        if (!duration.valid) {
            return 2;
        }
        pit_sleep_ms(duration.value);
    }
    else if(strcmp(argv[1], "-k") == 0) {
        term_write("Press any key to continue.\n", TC_WHITE);
        asm("hlt");
    }
    else {
        return 2;
    }

    return 0;
}