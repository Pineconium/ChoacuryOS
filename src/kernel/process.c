#include "process.h"
#include "../drivers/vga.h"
#include "../shell/shell.h"

PCB process_table[MAX_PROCESSES];
int current_process = -1;

// Helper function to determine if there are any processes ready
bool has_ready_process() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].state == PROCESS_READY) {
            return true;
        }
    }
    return false;
}

void context_switch(PCB* next_process) {
    if (current_process >= 0) {
        asm volatile("mov %%esp, %0" : "=r"(process_table[current_process].stack_pointer));
    }

    current_process = next_process - process_table;

    term_write("Switching to new process...\n", TC_YELLO);
    asm volatile("mov %0, %%esp" : : "r"(next_process->stack_pointer));
    next_process->program_counter();  // Now it correctly jumps to the function
}

bool all_processes_terminated() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].state != PROCESS_TERMINATED) {
            return false; // At least one process is still running
        }
    }
    return true; // All processes are terminated
}

void scheduler() {
    if (!has_ready_process()) {
        term_write("No processes ready, returning to shell.\n", TC_LRED);
        shell_start(); // Return control to the shell when no processes are ready
        return;
    }

    int next_process = (current_process + 1) % MAX_PROCESSES;

    while (process_table[next_process].state != PROCESS_READY) {
        next_process = (next_process + 1) % MAX_PROCESSES;
        if (next_process == current_process) {
            return;
        }
    }

    context_switch(&process_table[next_process]);
}

void create_process(void (*entry_point)(), process_priority_t priority) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].state == PROCESS_TERMINATED) {
            process_table[i].pid = i;
            process_table[i].stack_pointer = kmalloc(4096) + 4096;
            process_table[i].program_counter = entry_point;
            process_table[i].state = PROCESS_READY;
            process_table[i].priority = priority;

            term_write("Process created successfully.\n", TC_GREEN);
            return;
        }
    }
    term_write("Failed to create process: No available slots.\n", TC_LRED);
}

void terminate_process() {
    term_write("Terminating foreground process...\n", TC_LRED);
    process_table[current_process].state = PROCESS_TERMINATED;

    // Try to find the next process
    if (has_ready_process()) {
        scheduler();
    } else {
        // If no processes are left, return to the shell
        term_write("No processes left, returning to shell.\n", TC_LRED);
        shell_start();
    }
}