#include "./recovery.h"
#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"
#include "../../../recovery/recovery.h"
#include "../../../drivers/vbe.h"

int shell_recovery_command(int argc, const char** argv) {
    /* Initialize graphics mode and start recovery */
	BgaSetVideoMode(FHD,VBE_DISPI_BPP_32,1,1);
    start_recovery();

    /* If recovery exits, reinitialize text mode and render terminal */
	vga_text_init(TC_BLACK);
    term_rerender_buffer();

    return 0;
}