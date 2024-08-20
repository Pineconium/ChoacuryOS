#include "compdate.h"

#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"

int shell_compdate_command(int argc, const char** argv) {
    term_write(__DATE__ "\n", TC_WHITE);
}