#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#define MAX_PROG_LEN 12 // Max terms in a "program"
#define MAX_PROCS 20 // Max number of processes
#define QUANTUM 40 // Time quantum, ms

#define MIN(x,y) ((x)<(y)?(x):(y)) // Compute the minimum

enum proc_state {
    READY,
    SLEEPING,
    EXITED
};

struct process {
    int pid;
    enum proc_state state;
    int time_awake_remaining;
    int time_sleep_remaining;
    int program_counter;
    int program[MAX_PROG_LEN];
};

struct process table[MAX_PROCS];

void init_proc_table(void)
{
    for (int i = 0; i < MAX_PROCS; i++) {
        table[i].pid = i;
        table[i].state = EXITED;
        table[i].time_awake_remaining = 0;
        table[i].time_sleep_remaining = 0;
        table[i].program_counter = 0;
        
        for (int j = 0; j < MAX_PROG_LEN; j++) {
            table[i].program[j] = 0;
        }
    }
}

int parse_command_line(int argc, char **argv)
{
    int proc_count = 0;

    for (int i = 1; i < argc && proc_count < MAX_PROCS; i++) {
        char *token;
        int prog_index = 0;

        if ((token = strtok(argv[i], ",")) != NULL) do {
            if (prog_index < MAX_PROG_LEN -1) {
                table[proc_count].program[prog_index] = atoi(token);
                prog_index++;
            }
        } while ((token = strtok(NULL, ",")) != NULL);

        table[proc_count].program[prog_index] = 0;
        table[proc_count].program_counter = 0;
        table[proc_count].time_awake_remaining = table[proc_count].program[0];
        table[proc_count].time_sleep_remaining = 0;
        table[proc_count].state = READY;

        proc_count++;
    }
    return proc_count;
}

void enqueue_ready_processes(struct queue *q, int proc_count)
{
    for (int i = 0; i < proc_count; i++) {
        if (table[i].state == READY && table[i].time_awake_remaining > 0) {
            queue_enqueue(q, &table[i]);
        }
    }
}

int all_exited(int proc_count)
{
    for (int i = 0; i < proc_count; i++) {
        if (table[i].state != EXITED) {
            return 0;
        }
    }

    return 1;
}

void decrement_sleep_times(int amount, struct process *current_proc)
{
    for (int i = 0; i < MAX_PROCS; i++) {
        if (&table[i] != current_proc && table[i].state == SLEEPING) {
            table[i].time_sleep_remaining -= amount;
        }
    }
}

int find_next_wake_time(int proc_count)
{
    int first_sleep = 1;
    int next_wake = 0;

    for (int i = 0; i < proc_count; i++) {
        if (table[i].state == SLEEPING) {
            if (first_sleep || table[i].time_sleep_remaining < next_wake) {
                next_wake = table[i].time_sleep_remaining;
                first_sleep = 0;
            }
        }
    }

    return next_wake;
}

void wake_sleeping_processes(struct queue *q, int proc_count)
{
    for (int i = 0; i < proc_count; i++) {
        if (table[i].state == SLEEPING && table[i].time_sleep_remaining <= 0) {
            table[i].program_counter++;

            if (table[i].program[table[i].program_counter] == 0) {
                table[i].state = EXITED;
                printf("PID %d: Exiting\n", table[i].pid);
            }
            else {
                table[i].state = READY;
                table[i].time_awake_remaining = table[i].program[table[i].program_counter];
                printf("PID %d: Waking up for %d ms\n",
                       table[i].pid,
                       table[i].time_awake_remaining);
                queue_enqueue(q, &table[i]);
            }
        }
    }
}

void run_process(struct queue *q, struct process *current_proc, int *clock)
{
    int run_time = MIN(QUANTUM, current_proc->time_awake_remaining);

    current_proc->time_awake_remaining -= run_time;
    decrement_sleep_times(run_time, current_proc);
    *clock += run_time;

    if (current_proc->time_awake_remaining > 0) {
        queue_enqueue(q, current_proc);
    }
    else {
        current_proc->program_counter++;

        if (current_proc->program[current_proc->program_counter] == 0) {
            current_proc->state = EXITED;
            printf("PID %d: Exiting\n", current_proc->pid);
        }
        else {
            current_proc->state = SLEEPING;
            current_proc->time_sleep_remaining = current_proc->program[current_proc->program_counter];
            printf("PID %d: Sleeping for %d ms\n",
                   current_proc->pid,
                   current_proc->time_sleep_remaining);
        }
    }

    printf("PID %d: Ran for %d ms\n", current_proc->pid, run_time);
}

/**
 * Main.
 */
int main(int argc, char **argv)
{
    int clock = 0;
    int proc_count;

    struct queue *q = queue_new();

    init_proc_table();
    proc_count = parse_command_line(argc, argv);
    enqueue_ready_processes(q, proc_count);

    while (!all_exited(proc_count)) {
        if (queue_is_empty(q)) {
            int next_wake = find_next_wake_time(proc_count);
            decrement_sleep_times(next_wake, NULL);
            clock += next_wake;
        }

        printf("=== Clock %d ms ===\n", clock);
        wake_sleeping_processes(q, proc_count);

        struct process *current_proc = queue_dequeue(q);

        if (current_proc == NULL) {
            continue;
        }

        printf("PID %d: Running\n", current_proc->pid);
        run_process(q, current_proc, &clock);
    }

    queue_free(q);
}
