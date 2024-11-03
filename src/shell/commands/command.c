#include "command.h"

// Command includes
#include "help/help.c"
#include "guiload/guiload.h"
#include "clear/clear.h"
#include "beep/beep.h"
#include "calc/calc.h"
#include "compdate/compdate.h"
#include "echo/echo.h"
#include "pause/pause.h"
#include "pl/pl.h"
#include "chstat/chstat.h"
#include "cd/cd.h"
#include "cat/cat.h"
#include "ls/ls.h"
#include "vbetest/vbetest.h"
#include "whereami/whereami.h"

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
    {"cat",      {NULL},        "[str:file]", "Print a file contents.",                      shell_cat_command}, // DONE
    {"cd",       {NULL},        "[str:directory]", "Changes the current directory.",              shell_cd_command}, // DONE
    {"compdate", {NULL},        "No args.", "Shows the compilation date.",                 shell_compdate_command}, // DONE
    {"clear",    {"cls", NULL}, "No args.", "Clears the screen.",                          shell_clear_command},
    {"echo",     {NULL},        "No args.", "Prints string to the console.",               shell_echo_command}, // DONE
    {"guiload",  {NULL},        "No args.", "Loads up the GUI (WIP!)",                     shell_guiload_command},
    {"ls",       {NULL},        "(str:directory)", "List files in a directory.",                  shell_ls_command}, // DONE
    {"pause",    {NULL},        "(-t int:time) (-k)", "Pauses the terminal until a keyboard input.", shell_pause_command}, // DONE
    {"pl",       {NULL},        "No args.", "Shows the connected data devices.",           shell_pl_command}, // DONE
    {"chstat",   {NULL},        "No args.", "Display system information.",                 shell_chstat_command}, // DONE
    {"vbetest",  {NULL},        "No args.", "Test Bochs VBE extensions",                   shell_vbetest_command}, // DONE
    {"whereami", {"pwd", NULL}, "No args.", "Prints the current directory",                shell_whereami_command}  // DONE
};
int shell_commands_count = sizeof(shell_commands_list) / sizeof(Command);