#include "beep.h"

#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"
#include "../../../gui/desktop.h"

int shell_beep_command(int argc, const char** argv) {
    if (argc != 2 && argc != 3) {
        //term_write("ERROR: Usage -> beep [freq.] [duration]\n", TC_LRED);
        return 2;
    }

    atoi_result_t frequency = atoi(argv[1]);
    if (!frequency.valid) {
        //term_write("ERROR: Frequency provided is not an interger\n", TC_LRED);
        return 2;
    }

        // Use the default beep length if no value has been set by the user
    atoi_result_t duration = { .valid = true, .value = 500 };    // <-- 500 ms
    if (argc >= 3) {
        duration = atoi(argv[2]);
    }
    if (!duration.valid) {
        //term_write("ERROR: Duration provided is not an interger\n", TC_LRED);
        return 2;
    }

    startbeep(frequency.value);
    pit_sleep_ms(duration.value);
    mutebeep();
    
    return 0;
}