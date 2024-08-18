#include "command.h"

// Command includes
#include "help/help.c"
#include "guiload/guiload.h"

// Temp
int example_command(int argc, const char** argv) {

}

int temp_shell_guiload_command(int argc, char** argv) {
    return 0;
}

int temp_shell_notimplemented_command(int argc, char** argv) {
    return 0;
}

void shell_init_commands_list() {
    /*static Command shell_commands_list[] = {
        //{"example", "arg1 arg2", "Example command", example_command},
        {"guiload", "No args.", "Loads up the GUI (WIP!)", shell_guiload_command}
    };*/
}

Command shell_commands_list[] = {
    //{"example", (char*[]){"ex", "e.g."}, "arg1 arg2", "Example command", example_command},
    {"help", (char*[]){NULL}, "<command name>, (-a | --args)", "Shows this screen", shell_help_command},
    {"guiload", (char*[]){NULL}, "No args.", "Loads up the GUI (WIP!)", shell_guiload_command}, // It complains about expecting an expression, ignore it
    {"clear", (char*[]){"cls", NULL}, "No args.", "Clears the screen", temp_shell_notimplemented_command}
};
int shell_commands_count = sizeof(shell_commands_list) / sizeof(Command);