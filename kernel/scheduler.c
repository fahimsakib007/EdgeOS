#include "../include/scheduler.h"
#include "../include/process.h"
#include "../include/debug.h"


/* Print unsigned integer */
static void print_uint(unsigned int value)
{
    char buffer[16];
    int i = 0;

    if (value == 0)
    {
        debug_write_char('0');
        return;
    }

    while (value > 0)
    {
        buffer[i] =
            '0' + (value % 10);

        value = value / 10;
        i++;
    }

    while (i > 0)
    {
        i--;

        debug_write_char(
            buffer[i]
        );
    }
}


/* Initialize scheduler */
void scheduler_init(void)
{
    debug_writeln(
        "[SCHED] Scheduler initialized."
    );
}


/*
 * Find highest-priority READY process
 * that has not already been selected.
 */
static int find_highest_priority_process(
    unsigned int *scheduled
)
{
    int count =
        process_get_count();

    int selected_index = -1;

    unsigned int highest_priority = 0;

    int i;

    for (i = 0; i < count; i++)
    {
        if (scheduled[i])
        {
            continue;
        }

        if (process_get_state(i)
            != PROCESS_READY)
        {
            continue;
        }

        unsigned int priority =
            process_get_priority(i);

        if (selected_index == -1 ||
            priority > highest_priority)
        {
            selected_index = i;

            highest_priority =
                priority;
        }
    }

    return selected_index;
}


/*
 * =========================================
 * PRIORITY SCHEDULING
 * =========================================
 */
void scheduler_run_priority(void)
{
    unsigned int scheduled[MAX_PROCESSES];

    int i;

    for (i = 0;
         i < MAX_PROCESSES;
         i++)
    {
        scheduled[i] = 0;
    }

    debug_writeln("");

    debug_writeln(
        "================================="
    );

    debug_writeln(
        "[SCHED] Priority Scheduling"
    );

    debug_writeln(
        "================================="
    );

    int order = 1;

    while (1)
    {
        int selected =
            find_highest_priority_process(
                scheduled
            );

        if (selected == -1)
        {
            break;
        }

        scheduled[selected] = 1;

        process_set_state(
            selected,
            PROCESS_RUNNING
        );

        debug_write(
            "[SCHED] Order "
        );

        print_uint(order);

        debug_write(
            " -> PID "
        );

        print_uint(
            process_get_pid(selected)
        );

        debug_write(
            " | "
        );

        const char *name =
            process_get_name(selected);

        if (name != 0)
        {
            debug_write(name);
        }
        else
        {
            debug_write("UNKNOWN");
        }

        debug_write(
            " | Priority="
        );

        print_uint(
            process_get_priority(selected)
        );

        debug_writeln(
            " | RUNNING"
        );

        /*
         * Return process to READY after
         * demonstration execution.
         */
        process_set_state(
            selected,
            PROCESS_READY
        );

        order++;
    }

    debug_writeln(
        "================================="
    );

    debug_writeln(
        "[SCHED] Priority scheduling completed."
    );
}


/*
 * =========================================
 * ROUND ROBIN SCHEDULING
 * =========================================
 *
 * Each READY process receives one simulated
 * CPU time slice during every round.
 */
void scheduler_run_round_robin(
    unsigned int rounds
)
{
    int count =
        process_get_count();

    unsigned int round;

    debug_writeln("");

    debug_writeln(
        "================================="
    );

    debug_writeln(
        "[SCHED] Round Robin Scheduling"
    );

    debug_writeln(
        "================================="
    );

    for (round = 1;
         round <= rounds;
         round++)
    {
        debug_write(
            "[SCHED] Round "
        );

        print_uint(round);

        debug_writeln(":");

        int i;

        for (i = 0;
             i < count;
             i++)
        {
            if (process_get_state(i)
                != PROCESS_READY)
            {
                continue;
            }

            /*
             * Give this process
             * one simulated CPU time slice.
             */
            process_set_state(
                i,
                PROCESS_RUNNING
            );

            debug_write(
                "  [TIME SLICE] PID "
            );

            print_uint(
                process_get_pid(i)
            );

            debug_write(
                " | "
            );

            const char *name =
                process_get_name(i);

            if (name != 0)
            {
                debug_write(name);
            }
            else
            {
                debug_write("UNKNOWN");
            }

            debug_writeln(
                " | RUNNING"
            );

            /*
             * Time quantum ends.
             * Put process back into READY state.
             */
            process_set_state(
                i,
                PROCESS_READY
            );
        }
    }

    debug_writeln(
        "================================="
    );

    debug_writeln(
        "[SCHED] Round Robin scheduling completed."
    );
}
