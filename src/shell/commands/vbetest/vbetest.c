#include "vbetest.h"

#include "../../shell.h"
#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"

int shell_vbetest_command(int argc, const char** argv) {
    BgaSetVideoMode(FHD,VBE_DISPI_BPP_32,1,1);
    return 0;
}