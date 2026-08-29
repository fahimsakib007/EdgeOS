#include "../include/process.h"
#include "../include/memory.h"
#include "../include/debug.h"


typedef struct
{
    int pid;

    const char *name;

    unsigned int priority;

    unsigned int state;

    unsigned int memory_size;

    void *memory_ptr;

} PCB;


static PCB process_table[MAX_PROCESSES];

static int next_pid = 1;


/* Small integer printer for debug output */
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


/* Print process state */
static void print_state(
    unsigned int state
)
{
    if (state == PROCESS_READY)
    {
        debug_write("READY");
    }
    else if (state == PROCESS_RUNNING)
    {
        debug_write("RUNNING");
    }
    else if (state == PROCESS_WAITING)
    {
        debug_write("WAITING");
    }
    else if (state == PROCESS_TERMINATED)
    {
        debug_write("TERMINATED");
    }
    else
    {
        debug_write("UNUSED");
    }
}


/* Initialize process table */
void process_init(void)
{
    int i;

    for (i = 0;
         i < MAX_PROCESSES;
         i++)
    {
        process_table[i].pid = 0;

        process_table[i].name = 0;

        process_table[i].priority = 0;

        process_table[i].state =
            PROCESS_UNUSED;

        process_table[i].memory_size = 0;

        process_table[i].memory_ptr = 0;
    }

    next_pid = 1;

    debug_writeln(
        "[PROC] Process manager initialized."
    );
}


/* Create a new process */
int process_create(
    const char *name,
    unsigned int memory_size,
    unsigned int priority
)
{
    int i;

    for (i = 0;
         i < MAX_PROCESSES;
         i++)
    {
        if (process_table[i].state ==
            PROCESS_UNUSED)
        {
            void *memory =
                kmalloc(memory_size);

            if (memory == 0)
            {
                debug_writeln(
                    "[PROC] Process creation failed: no memory."
                );

                return -1;
            }

            process_table[i].pid =
                next_pid++;

            process_table[i].name =
                name;

            process_table[i].priority =
                priority;

            process_table[i].state =
                PROCESS_READY;

            process_table[i].memory_size =
                memory_size;

            process_table[i].memory_ptr =
                memory;

            debug_write(
                "[PROC] Created process PID "
            );

            print_uint(
                process_table[i].pid
            );

            debug_write(": ");

            debug_writeln(name);

            return process_table[i].pid;
        }
    }

    debug_writeln(
        "[PROC] Process table full."
    );

    return -1;
}


/* Terminate a process */
void process_terminate(int pid)
{
    int i;

    for (i = 0;
         i < MAX_PROCESSES;
         i++)
    {
        if (process_table[i].pid == pid &&
            process_table[i].state !=
            PROCESS_UNUSED)
        {
            if (process_table[i].memory_ptr)
            {
                kfree(
                    process_table[i].memory_ptr
                );
            }

            process_table[i].state =
                PROCESS_TERMINATED;

            process_table[i].memory_ptr = 0;

            debug_write(
                "[PROC] Terminated PID "
            );

            print_uint(pid);

            debug_writeln("");

            return;
        }
    }

    debug_writeln(
        "[PROC] PID not found."
    );
}


/* Print PCB / process table */
void process_print_table(void)
{
    int i;

    debug_writeln("");

    debug_writeln(
        "================================="
    );

    debug_writeln(
        "[PROC] EdgeOS Process Table"
    );

    debug_writeln(
        "================================="
    );

    for (i = 0;
         i < MAX_PROCESSES;
         i++)
    {
        if (process_table[i].state !=
            PROCESS_UNUSED)
        {
            debug_write("PID=");

            print_uint(
                process_table[i].pid
            );

            debug_write(" NAME=");

            debug_write(
                process_table[i].name
            );

            debug_write(" PRIORITY=");

            print_uint(
                process_table[i].priority
            );

            debug_write(" MEMORY=");

            print_uint(
                process_table[i].memory_size
            );

            debug_write(" STATE=");

            print_state(
                process_table[i].state
            );

            debug_writeln("");
        }
    }

    debug_writeln(
        "================================="
    );
}


/*
 * =====================================================
 * Scheduler Interface
 * =====================================================
 *
 * These functions allow scheduler.c to inspect
 * process information without directly accessing
 * process_table.
 */


/* Return maximum number of process slots */
int process_get_count(void)
{
    return MAX_PROCESSES;
}


/* Get PID from process-table index */
int process_get_pid(int index)
{
    if (index < 0 ||
        index >= MAX_PROCESSES)
    {
        return -1;
    }

    return process_table[index].pid;
}


/* Get process name */
const char *process_get_name(int index)
{
    if (index < 0 ||
        index >= MAX_PROCESSES)
    {
        return 0;
    }

    return process_table[index].name;
}


/* Get process priority */
unsigned int process_get_priority(int index)
{
    if (index < 0 ||
        index >= MAX_PROCESSES)
    {
        return 0;
    }

    return process_table[index].priority;
}


/* Get current process state */
unsigned int process_get_state(int index)
{
    if (index < 0 ||
        index >= MAX_PROCESSES)
    {
        return PROCESS_UNUSED;
    }

    return process_table[index].state;
}


/* Change process state */
void process_set_state(
    int index,
    unsigned int state
)
{
    if (index < 0 ||
        index >= MAX_PROCESSES)
    {
        return;
    }

    process_table[index].state =
        state;
}
