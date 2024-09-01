#include "./guiload.h"
#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"
#include "../../../gui/desktop.h"
#include "../../../drivers/vbe.h"

int shell_guiload_command(int argc, const char** argv) {
    /* Initialize graphics mode and start desktop */
	BgaSetVideoMode(FHD,VBE_DISPI_BPP_32,1,1);
    start_desktop();

    /* If desktop exits, reinitialize text mode and render terminal */
	vga_text_init(TC_BLACK);
    term_rerender_buffer();

    return 0;
}