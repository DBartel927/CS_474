#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#define MAX_PROCS 10 // Max number of processes
#define QUANTUM 40 // Time quantum, ms
#define MIN(x,y) ((x)<(y)?(x):(y)) // Compute the minimum

/**
 * Process information.
 */
struct process {
    int pid;
    int time_awake_remaining;
};

/**
 * The process table.
 */
struct process table[MAX_PROCS];

/**
 * Initialize the process table.
 */
void init_proc_table(void)
{
    for (int i = 0; i < MAX_PROCS; i++) {
        table[i].pid = i;
        table[i].time_awake_remaining = 0;
    }
}

/**
 * Parse the command line.
 */
void parse_command_line(int argc, char **argv)
{
    for (int i = 1; i < argc && i <= MAX_PROCS; i++) {
        table[i-1].time_awake_remaining = atoi(argv[i]);
    }
}

/**
 * Main.
 */
int main(int argc, char **argv)
{
    struct queue *q = queue_new();

    init_proc_table();

    parse_command_line(argc, argv);

    for (int i = 0; i < MAX_PROCS; i++) {
        if (table[i].time_awake_remaining > 0) {
            queue_enqueue(q, &table[i]);
        }
    }

    int clock = 0;
    struct process *current_proc;

    while ((current_proc = queue_dequeue(q)) != NULL) {
        printf("=== Clock %d ms ===\n", clock);
        printf("PID %d: Running\n", current_proc->pid);

        int run_time = MIN(QUANTUM, current_proc->time_awake_remaining);
        current_proc->time_awake_remaining -= run_time;
        clock += run_time;
        printf("PID %d: Ran for %d ms\n", current_proc->pid, run_time);

        if (current_proc->time_awake_remaining > 0) {
            queue_enqueue(q, current_proc);
        }
    }

    queue_free(q);
}
