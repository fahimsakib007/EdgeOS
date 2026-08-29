#include "../include/resource.h"
#include "../include/memory.h"
#include "../include/process.h"
#include "../include/debug.h"


#define WARNING_FREE_MEMORY  (400 * 1024)
#define CRITICAL_FREE_MEMORY (200 * 1024)


/*
 * Small integer printer
 */
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

        value =
            value / 10;

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


/*
 * Initialize EdgeOS Resource Manager
 */
void resource_init(void)
{
    debug_writeln(
        "[RES] Resource manager initialized."
    );

    debug_writeln(
        "[RES] Memory-pressure monitoring enabled."
    );
}


/*
 * Determine current memory-pressure level.
 *
 * NORMAL:
 *     More than 400 KB free
 *
 * WARNING:
 *     200 KB - 400 KB free
 *
 * CRITICAL:
 *     Less than 200 KB free
 */
unsigned int resource_get_memory_pressure(void)
{
    unsigned int free_memory =
        memory_get_free();


    if (free_memory <
        CRITICAL_FREE_MEMORY)
    {
        return
            MEMORY_PRESSURE_CRITICAL;
    }


    if (free_memory <
        WARNING_FREE_MEMORY)
    {
        return
            MEMORY_PRESSURE_WARNING;
    }


    return
        MEMORY_PRESSURE_NORMAL;
}


/*
 * Display current resource condition
 */
void resource_print_status(void)
{
    unsigned int used_memory =
        memory_get_used();

    unsigned int free_memory =
        memory_get_free();

    unsigned int fragmentation =
        memory_get_fragmentation();

    unsigned int pressure =
        resource_get_memory_pressure();


    debug_writeln("");

    debug_writeln(
        "================================="
    );

    debug_writeln(
        "[RES] EdgeOS Resource Monitor"
    );

    debug_writeln(
        "================================="
    );


    debug_write(
        "[RES] Used memory: "
    );

    print_uint(
        used_memory
    );

    debug_writeln(
        " bytes"
    );


    debug_write(
        "[RES] Free memory: "
    );

    print_uint(
        free_memory
    );

    debug_writeln(
        " bytes"
    );


    debug_write(
        "[RES] Fragmentation: "
    );

    print_uint(
        fragmentation
    );

    debug_writeln(
        "%"
    );


    debug_write(
        "[RES] Memory pressure: "
    );


    if (pressure ==
        MEMORY_PRESSURE_NORMAL)
    {
        debug_writeln(
            "NORMAL"
        );
    }
    else if (pressure ==
             MEMORY_PRESSURE_WARNING)
    {
        debug_writeln(
            "WARNING"
        );
    }
    else
    {
        debug_writeln(
            "CRITICAL"
        );
    }


    debug_writeln(
        "================================="
    );
}


/*
 * Find a low-priority process and
 * reclaim its memory during
 * critical memory pressure.
 */
static int reclaim_low_priority_process(void)
{
    int count =
        process_get_count();

    int i;


    for (i = 0;
         i < count;
         i++)
    {
        if (process_get_state(i) ==
                PROCESS_READY &&
            process_get_priority(i) ==
                PRIORITY_LOW)
        {
            int pid =
                process_get_pid(i);

            const char *name =
                process_get_name(i);


            debug_write(
                "[RES] Reclaiming LOW priority process: "
            );


            if (name != 0)
            {
                debug_writeln(
                    name
                );
            }
            else
            {
                debug_writeln(
                    "UNKNOWN"
                );
            }


            process_terminate(
                pid
            );


            debug_writeln(
                "[RES] Process memory reclaimed."
            );


            return 1;
        }
    }


    return 0;
}


/*
 * React automatically to
 * memory-pressure conditions.
 */
void resource_handle_memory_pressure(void)
{
    unsigned int pressure =
        resource_get_memory_pressure();


    if (pressure ==
        MEMORY_PRESSURE_NORMAL)
    {
        debug_writeln(
            "[RES] Memory pressure NORMAL."
        );

        debug_writeln(
            "[RES] No resource action required."
        );

        return;
    }


    if (pressure ==
        MEMORY_PRESSURE_WARNING)
    {
        debug_writeln(
            "[RES] WARNING: Memory pressure detected."
        );

        debug_writeln(
            "[RES] Monitoring low-priority processes."
        );

        return;
    }


    /*
     * CRITICAL MEMORY PRESSURE
     */
    debug_writeln(
        "[RES] CRITICAL memory pressure!"
    );

    debug_writeln(
        "[RES] Protecting high-priority workloads."
    );

    debug_writeln(
        "[RES] Searching for reclaimable process..."
    );


    if (!reclaim_low_priority_process())
    {
        debug_writeln(
            "[RES] No LOW priority process available."
        );
    }
}
