#ifndef PROCESS_H
#define PROCESS_H

#include "../drivers/types.h"
#include "../memory/kmalloc.h"
#include "../shell/terminal.h"
#include "../drivers/pit.h"

#define MAX_PROCESSES 20

#define PROCESS_READY    0
#define PROCESS_RUNNING  1
#define PROCESS_WAITING  2
#define PROCESS_BLOCKED  3
#define PROCESS_TERMINATED 4

typedef enum {
    HIGH_PRIORITY,
    LOW_PRIORITY,
} process_priority_t;

typedef struct {
    uint32_t pid;              // Process ID
    uint32_t* stack_pointer;   // Stack pointer
    void (*program_counter)(); // Program counter
    uint32_t state;            // Process state
    process_priority_t priority;         // Process priority
    uint32_t waiting_time;     // Waiting time
    uint32_t turnaround_time;  // Turnaround time
} PCB;

extern PCB process_table[MAX_PROCESSES];
extern int current_process;
extern int foreground_process;

void scheduler();
void context_switch(PCB* next_process);
void create_process(void (*entry_point)(), process_priority_t priority);
void terminate_process();
bool all_processes_terminated();

#endif