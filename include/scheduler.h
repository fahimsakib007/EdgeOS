#ifndef SCHEDULER_H
#define SCHEDULER_H

void scheduler_init(void);

/* Priority Scheduling */
void scheduler_run_priority(void);

/* Round Robin Scheduling */
void scheduler_run_round_robin(
    unsigned int rounds
);

#endif
