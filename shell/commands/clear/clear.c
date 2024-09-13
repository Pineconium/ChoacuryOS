#include "clear.h"
#include "../../terminal.h"

int shell_clear_command(int argc, const char** argv) {
    term_clear();

    return 0;
}