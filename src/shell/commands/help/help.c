#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"
#include "../../../gui/desktop.h"
#include "../command.h"

// Put used_alias as -1 or 17 to set it as nothing btw
void print_specific_command_help(int argc, const char** argv, Command command, int used_alias) {
    term_write("Information for command ", TC_WHITE);
    term_write(command.name, TC_BRIGHT);
    term_write("\n", TC_WHITE);

    term_write("Command description: ", TC_WHITE);
    term_write(command.description, TC_BRIGHT);
    term_write("\n\n", TC_WHITE);

    term_write("Command usage:\nAliases (Includes name): ", TC_WHITE);
    term_write(command.name, used_alias == -1 ? TC_LBLUE : TC_BRIGHT);
    for (size_t j = 0; command.aliases[j] != NULL; j++)
    {
        term_write(", ", TC_WHITE);
        term_write(command.aliases[j], used_alias == j ? TC_LBLUE : TC_BRIGHT);
    }
                
    term_write("\n\nArgument information: ", TC_WHITE);
    term_write(command.args, TC_BRIGHT);
    term_write("\n", TC_WHITE);
}

int shell_help_command(int argc, const char** argv) {
    if(argc == 1) {
        // Display the standard help screen
        term_write("Commands list:\n", TC_WHITE);
        //term_write("Attach the -a or --args flag to the help command for argument information.\n", TC_WHITE);
        term_write("Use 'help <command name/alias>' for command-specific information.\n", TC_BRIGHT);

        term_write("Command             Description\n", TC_LBLUE);

        for (size_t i = 0; i < shell_commands_count; i++) {
            term_write(shell_commands_list[i].name, TC_WHITE);
            int cmdlength = strlen(shell_commands_list[i].name);
            for (size_t j = 0; shell_commands_list[i].aliases[j] != NULL; j++)
            {
                term_write(", ", TC_WHITE);
                term_write(shell_commands_list[i].aliases[j], TC_WHITE);

                cmdlength += strlen(shell_commands_list[i].aliases[j]) + 2; // + 2 for the comma and space
            }

            for (size_t i = 0; i < (20 - cmdlength); i++)
            {
                term_write(" ", TC_WHITE);
            }

            //term_write(" - ", TC_WHITE);
            term_write(shell_commands_list[i].description, TC_WHITE);

            term_write("\n", TC_WHITE);
        }
    } else if(argc == 2) {
        for (size_t i = 0; i < shell_commands_count; i++)
        {
            if(strcmp(argv[1], shell_commands_list[i].name) == 0) {
                // Found command

                /*term_write("Information for command ", TC_WHITE);
                term_write(shell_commands_list[i].name, TC_BRIGHT);
                term_write("\n", TC_WHITE);

                term_write("Command description: ", TC_WHITE);
                term_write(shell_commands_list[i].description, TC_BRIGHT);
                term_write("\n\n", TC_WHITE);

                term_write("Command usage:\nAliases (Includes name): ", TC_WHITE);
                term_write(shell_commands_list[i].name, TC_BRIGHT);
                for (size_t j = 0; shell_commands_list[i].aliases[j] != NULL; j++)
                {
                    term_write(", ", TC_WHITE);
                    term_write(shell_commands_list[i].aliases[j], TC_BRIGHT);
                }

                term_write("\n\nArgument information: ", TC_WHITE);
                term_write(shell_commands_list[i].args, TC_BRIGHT);
                term_write("\n", TC_WHITE);*/

                print_specific_command_help(argc, argv, shell_commands_list[i], -1);

                return 0;
            }

            for (size_t j = 0; shell_commands_list[i].aliases[j] != NULL; j++)
            {
                if(strcmp(argv[1], shell_commands_list[i].aliases[j]) == 0) {
                    // Found command

                    print_specific_command_help(argc, argv, shell_commands_list[i], j);

                    return 0;
                }
            }
        }

        // No command exists
        term_write("No command ", TC_YELLO);
        term_write(argv[1], TC_YELLO);
        term_write(" found, run '", TC_YELLO);
        term_write("help", TC_BRIGHT);
        term_write("' for the list of the available commands.\n", TC_YELLO);
        return 1;
    }
    
    return 0;
}