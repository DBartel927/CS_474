// A round robin scheduler has a queue of ready-to-run processes. (The queue in this case holds pointers to struct process structures with process information.)

// When it's time to actually start a process running and give it the CPU, the process information is dequeued, and the process runs until one of three things happens:

// It runs out of time and gets interrupted.
// When this happens, we enqueue the process once more.
// It goes to sleep blocking (e.g. on I/O).
// It exits.
// There are actually more than three things, but we'll keep it simple this week.
// The effect is that if you have multiple processes, they'll line up and the one at the front of the line runs. If it gets interrupted and needs more time, it goes to the back of the line and waits to run again.

// For the simplest demonstration of this effect, let's enqueue two processes. Then, in a loop, let's dequeue one of them, simulate that it's running (by printing something to that effect), and then enqueue it again after it's done.

// We should see the two processes running round-robin.

// The Process Table
// The main owner of all the process information is the process table. In our simulation, it's an array of struct processs. Examine what's there. It looks like overkill now, but this is the basis for this week's project, so don't you worry—it'll get substantially more complex. 😊

// Processes in the table are identified by their index. PID 3 is at index 3, etc.

// The Queue
// The queue holds pointers to process table entries that are ready-to-run. (In the future, we'll have process table entries that are sleeping or exited, and those entries will not be in the queue.) So processes that are ready to run get added to the queue, and when they're no longer ready to run (sleeping or exited), they do not get readded.

// If they use up their time slice and still need to run, they do get readded.

// The queue and process table are separate entities, but the elements in the queue point to the entries in the process table.

// What to Do
// Enqueue PID 0 and PID 1.

// Loop 15 times, and each time:

// dequeue a process (from the front of the queue).
// printf("PID %d: Running\n", p->pid);
// enqueue the process again (to the back of the queue).
// Example Run
// PID 0: Running
// PID 1: Running
// PID 0: Running
// PID 1: Running
// PID 0: Running
// PID 1: Running
// PID 0: Running
// PID 1: Running
// PID 0: Running
// PID 1: Running
// PID 0: Running
// PID 1: Running
// PID 0: Running
// PID 1: Running
// PID 0: Running

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#define MAX_PROCS 10 // Max number of processes

/**
 * Process information.
 */
struct process {
    int pid;
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
    }
}

/**
 * Main.
 */
int main(void)
{
    struct queue *q = queue_new();

    init_proc_table();

    queue_enqueue(q, &table[0]);
    queue_enqueue(q, &table[1]);

    for (int i = 0; i < 15; i++) {
        struct process *p = queue_dequeue(q);
        printf("PID %d: Running\n", p->pid);
        queue_enqueue(q, p);
    }

    queue_free(q);
}
