#include "chstat.h"

#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"
#include "../../../memory/pmm.h"

int shell_chstat_command(int argc, const char** argv) {
    // this is basically a stupid neofetch clone
        char mem_mib_buffer[20];
        uint64_to_string(g_total_pmm_bytes / 1024 / 1024, mem_mib_buffer);
        mem_mib_buffer[19] = 0;                                 // <-- to prevent undefined behaviour
        char cpu_vendor[13];
        char cpu_brand[49];


        get_cpu_info(cpu_vendor, cpu_brand);

        term_write("BUILD: ", TC_LBLUE);
        term_write(__DATE__ " @ " __TIME__ "\n", TC_WHITE);
        term_write("KERNEL: ", TC_LBLUE);
        term_write("Choacury Standard\n", TC_WHITE);            // <-- aka. stock kernel.
        term_write("SHELL: ", TC_LBLUE);
        term_write("chsh-0.0.0.0041e-dev\n", TC_WHITE);         // <-- Could be more automated ngl.
        term_write("RAM: ", TC_LBLUE);
        term_write(mem_mib_buffer, TC_WHITE);
        term_write(" MiB\n", TC_WHITE);
        term_write("CPU Vendor: ", TC_LBLUE);
        term_write(cpu_vendor, TC_WHITE);
        term_write("\n", TC_WHITE);

        term_write("CPU Brand: ", TC_LBLUE);
        term_write(cpu_brand[0] ? cpu_brand : "N/A", TC_WHITE);  // If brand is empty, show "N/A"
        term_write("\n", TC_WHITE);
}