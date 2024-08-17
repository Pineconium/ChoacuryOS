#pragma once

//#include <guiload/guiload.h>

void shell_init_commands_list();

// Command type definition
typedef struct {
  char* name;                      // <-- Name of the command
  char** aliases;                  // <-- Other names for the command (E.g. cls for clear) - Displayed in help after name (Comma seperated)
  char* args;                      // <-- Argument information
  char* description;               // <-- Description of the command
  int (*func)(int, const char**);  // <-- The command returns a return code (Look in structure.md for more info on return codes)
} Command;

//extern Command shell_commands[];

int temp_shell_guiload_command(int argc, char** argv);

/*extern Command shell_commands_list[] = {
    //{"example", "arg1 arg2", "Example command", example_command},
    {"guiload", "No args.", "Loads up the GUI (WIP!)", temp_shell_guiload_command}
};*/

//extern Command* shell_commands_list;
extern Command shell_commands_list[];
extern int shell_commands_count;