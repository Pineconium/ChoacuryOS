#include "whereami.h"

#include "../../shell.h"
#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"

int shell_whereami_command(int argc, const char** argv) {
    term_write(currentDir, TC_WHITE);
    term_write("\n", TC_WHITE);

    return 0;
}