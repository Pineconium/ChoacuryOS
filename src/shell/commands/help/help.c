#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"
#include "../../../gui/desktop.h"
#include "../command.h"

int shell_help_command(int argc, const char** argv) {
    /**
     * Available flags:
     * -a, --args: Display argument information
     */

    if(argc == 1) {
        // Display the standard help screen
        term_write("Commands list:\n", TC_WHITE);
        term_write("Attach the -a or --args flag to the help command for argument information.\n", TC_WHITE);

        for (size_t i = 0; i < shell_commands_count; i++) {
            term_write(shell_commands_list[i].name, TC_WHITE);
            int cmdlength = strlen(shell_commands_list[i].name);
            for (size_t j = 0; j < shell_commands_list[i].aliases[j] != NULL; j++)
            {
                term_write(", ", TC_WHITE);
                term_write(shell_commands_list[i].aliases[j], TC_WHITE);

                cmdlength += strlen(shell_commands_list[i].aliases[j]);
            }

            for (size_t i = 0; i < (20 - cmdlength); i++)
            {
                //term_write(" ", TC_WHITE);
            }

            term_write(" - ", TC_WHITE);
            term_write(shell_commands_list[i].description, TC_WHITE);

            term_write("\n", TC_WHITE);
        }
    }
    
    return 0;
}