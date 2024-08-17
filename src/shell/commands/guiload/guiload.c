#include "./guiload.h"
#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"
#include "../../../gui/desktop.h"

int shell_guiload_command(int argc, const char** argv) {
    /* Initialize graphics mode and start desktop */
	vga_graphics_init(TC_BLUE);
    start_desktop();

    /* If desktop exits, reinitialize text mode and render terminal */
	vga_text_init(TC_BLACK);
    term_rerender_buffer();

    return 0;
}