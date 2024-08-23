#include "echo.h"

#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"

int shell_echo_command(int argc, const char** argv) {
    for (int i = 1; i < argc; i++) {
        if (i > 1)
            term_write(" ", TC_WHITE);
        term_write(argv[i], TC_WHITE);
    }
    term_write("\n", TC_WHITE);
}