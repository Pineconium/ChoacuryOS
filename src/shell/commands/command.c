#include "command.h"

// Command includes
#include "help/help.c"
#include "guiload/guiload.h"
#include "clear/clear.h"
#include "beep/beep.h"
#include "calc/calc.h"
#include "compdate/compdate.h"
#include "echo/echo.h"

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
    // Command   Alias(es)      Args        Description                                    Function
    {"help",     {NULL},        "<str:command name/alias> (args)", "Shows all of the available commands",    shell_help_command},
    {"beep",     {NULL},        "[int:freq.] [int:duration]", "PC Beeper control.",   shell_beep_command}, // DONE
    {"calc",     {NULL},        "[int:number1] [str:func] [int:number2]", "Literally a calculator.", shell_calc_command}, // DONE
    {"cat",      {NULL},        "No args.", "Print a file contents.",                      temp_shell_notimplemented_command}, // TODO
    {"cd",       {NULL},        "No args.", "Changes the current directory.",              temp_shell_notimplemented_command}, // TODO
    {"compdate", {NULL},        "No args.", "Shows the compilation date.",                 shell_compdate_command}, // DONE
    {"clear",    {"cls", NULL}, "No args.", "Clears the screen.",                          shell_clear_command},
    {"echo",     {NULL},        "No args.", "Prints string to the console.",               shell_echo_command}, // DONE
    {"guiload",  {NULL},        "No args.", "Loads up the GUI (WIP!)",                     shell_guiload_command},
    {"ls",       {NULL},        "No args.", "List files in a directory.",                  temp_shell_notimplemented_command}, // TODO
    {"pause",    {NULL},        "No args.", "Pauses the terminal until a keyboard input.", temp_shell_notimplemented_command}, // TODO
    {"pl",       {NULL},        "No args.", "Shows the connected data devices.",           temp_shell_notimplemented_command}, // TODO
    {"chstat",   {NULL},        "No args.", "Display system information.",                 temp_shell_notimplemented_command}, // TODO
    {"vbetest",  {NULL},        "No args.", "Test Bochs VBE extensions",                   temp_shell_notimplemented_command}, // TODO
    {"whereami", {NULL},        "No args.", "Prints the current directory",                temp_shell_notimplemented_command}  // TODO
};
int shell_commands_count = sizeof(shell_commands_list) / sizeof(Command);