#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 10

#define PRIORITY_LOW     1
#define PRIORITY_MEDIUM  2
#define PRIORITY_HIGH    3

#define PROCESS_UNUSED      0
#define PROCESS_READY       1
#define PROCESS_RUNNING     2
#define PROCESS_WAITING     3
#define PROCESS_TERMINATED  4

void process_init(void);

int process_create(
    const char *name,
    unsigned int memory_size,
    unsigned int priority
);

void process_terminate(int pid);

void process_print_table(void);

/* Scheduler interface */

int process_get_count(void);

int process_get_pid(int index);

const char *process_get_name(int index);

unsigned int process_get_priority(int index);

unsigned int process_get_state(int index);

void process_set_state(
    int index,
    unsigned int state
);

#endif
