#include "command.h"

// Command includes
#include "help/help.c"
#include "guiload/guiload.h"
#include "clear/clear.h"

// Temp
int example_command(int argc, const char** argv) {

}

int temp_shell_guiload_command(int argc, char** argv) {
    return 0;
}

int temp_shell_notimplemented_command(int argc, const char** argv) {
    return 0;
}

void shell_init_commands_list() {
    /*static Command shell_commands_list[] = {
        //{"example", "arg1 arg2", "Example command", example_command},
        {"guiload", "No args.", "Loads up the GUI (WIP!)", shell_guiload_command}
    };*/
}

/**
 * How should commands be defined?
 * Like this: {"command name", {"alias1", "alias2", NULL}, "Command description", link_to_command}
 * 
 * Arguments should be defined like this:
 * Short version with a single dash and from 1-2 letters, e.g. -a
 * Long/full version with two dashes, e.g. --args
 */

Command shell_commands_list[] = {
    //{"example", (char*[]){"ex", "e.g."}, "arg1 arg2", "Example command", example_command},
    {"help", {NULL}, "<command name/alias>", "Shows all of the available commands", shell_help_command},
    {"guiload", {NULL}, "No args.", "Loads up the GUI (WIP!)", shell_guiload_command}, // It complains about expecting an expression, ignore it
    {"clear", {"cls", NULL}, "No args.", "Clears the screen", shell_clear_command}
};
int shell_commands_count = sizeof(shell_commands_list) / sizeof(Command);