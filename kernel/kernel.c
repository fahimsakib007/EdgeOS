#include "../include/debug.h"
#include "../include/memory.h"
#include "../include/syscall.h"
#include "../include/process.h"
#include "../include/scheduler.h"
#include "../include/resource.h"


void kernel_main(void)
{
    debug_writeln("");
    debug_writeln("=========================================");
    debug_writeln("           EdgeOS v1.0");
    debug_writeln("   FINAL INTEGRATED OS DEMONSTRATION");
    debug_writeln("=========================================");
    debug_writeln("");

    debug_writeln("[BOOT] Kernel initialized successfully.");
    debug_writeln("[BOOT] Debug console initialized.");


    /*
     * =================================================
     * PHASE 1 — MEMORY MANAGEMENT
     * =================================================
     */

    debug_writeln("");
    debug_writeln("=========================================");
    debug_writeln("[DEMO 1] MEMORY MANAGEMENT");
    debug_writeln("=========================================");

    memory_init();

    debug_writeln("[DEMO] Allocating Block A: 100 bytes");
    void *a = kmalloc(100);

    debug_writeln("[DEMO] Allocating Block B: 200 bytes");
    void *b = kmalloc(200);

    debug_writeln("[DEMO] Allocating Block C: 300 bytes");
    void *c = kmalloc(300);

    memory_print();

    debug_writeln("[DEMO] Releasing A, B and C...");

    kfree(a);
    kfree(b);
    kfree(c);

    memory_print();

    debug_writeln(
        "[PASS] Allocation, splitting and coalescing working."
    );


    /*
     * =================================================
     * PHASE 2 — FRAGMENTATION OPTIMIZATION
     * =================================================
     */

    debug_writeln("");
    debug_writeln("=========================================");
    debug_writeln("[DEMO 2] FRAGMENTATION OPTIMIZATION");
    debug_writeln("=========================================");

    /*
     * Reset memory so this experiment starts
     * from a clean 1 MB memory pool.
     */
    memory_init();

    debug_writeln(
        "[DEMO] Allocating A = 200 KB"
    );

    a = kmalloc(
        200 * 1024
    );

    debug_writeln(
        "[DEMO] Allocating B = 300 KB"
    );

    b = kmalloc(
        300 * 1024
    );

    debug_writeln(
        "[DEMO] Allocating C = 100 KB"
    );

    c = kmalloc(
        100 * 1024
    );


    /*
     * Free A and C while B remains allocated.
     *
     * This creates separated free regions.
     */
    debug_writeln("");
    debug_writeln(
        "[DEMO] Creating fragmented memory..."
    );

    kfree(a);
    kfree(c);


    debug_writeln("");
    debug_writeln(
        "[BEFORE OPTIMIZATION]"
    );

    memory_print();


    /*
     * Freeing B makes all free regions adjacent.
     * EdgeOS coalescing can merge them.
     */
    debug_writeln(
        "[DEMO] Releasing middle Block B..."
    );

    kfree(b);


    debug_writeln("");
    debug_writeln(
        "[AFTER OPTIMIZATION]"
    );

    memory_print();

    debug_writeln(
        "[PASS] External fragmentation reduced."
    );


    /*
     * =================================================
     * PHASE 3 — SYSTEM CALL INTERFACE
     * =================================================
     */

    debug_writeln("");
    debug_writeln("=========================================");
    debug_writeln("[DEMO 3] SYSTEM CALL INTERFACE");
    debug_writeln("=========================================");

    memory_init();

    debug_writeln(
        "[APP] Requesting 256 bytes using SYS_MALLOC..."
    );

    unsigned int app_memory =
        syscall_dispatch(
            SYS_MALLOC,
            256
        );


    if (app_memory != 0)
    {
        debug_writeln(
            "[APP] SYS_MALLOC SUCCESS."
        );
    }
    else
    {
        debug_writeln(
            "[APP] SYS_MALLOC FAILED."
        );
    }


    memory_print();


    debug_writeln(
        "[APP] Releasing memory using SYS_FREE..."
    );

    syscall_dispatch(
        SYS_FREE,
        app_memory
    );


    memory_print();


    debug_writeln(
        "[DEMO] Testing invalid system call..."
    );

    syscall_dispatch(
        999,
        0
    );


    debug_writeln(
        "[PASS] System-call dispatcher working."
    );


    /*
     * =================================================
     * PHASE 4 — PROCESS MANAGEMENT
     * =================================================
     */

    debug_writeln("");
    debug_writeln("=========================================");
    debug_writeln("[DEMO 4] PROCESS MANAGEMENT / PCB");
    debug_writeln("=========================================");

    memory_init();
    process_init();


    int logger_pid =
        process_create(
            "Logger",
            16 * 1024,
            PRIORITY_LOW
        );


    int camera_pid =
        process_create(
            "Camera",
            64 * 1024,
            PRIORITY_HIGH
        );


    int sensor_pid =
        process_create(
            "Sensor",
            32 * 1024,
            PRIORITY_MEDIUM
        );


    (void)logger_pid;
    (void)camera_pid;
    (void)sensor_pid;


    process_print_table();

    memory_print();


    debug_writeln(
        "[PASS] PCB and process memory management working."
    );


    /*
     * =================================================
     * PHASE 5 — CPU SCHEDULING
     * =================================================
     */

    debug_writeln("");
    debug_writeln("=========================================");
    debug_writeln("[DEMO 5] CPU SCHEDULING");
    debug_writeln("=========================================");

    scheduler_init();


    debug_writeln("");
    debug_writeln(
        "[DEMO] PRIORITY SCHEDULING"
    );

    scheduler_run_priority();


    debug_writeln("");
    debug_writeln(
        "[DEMO] ROUND ROBIN SCHEDULING"
    );

    scheduler_run_round_robin(
        2
    );


    debug_writeln(
        "[PASS] Scheduler comparison working."
    );


    /*
     * =================================================
     * PHASE 6 — RESOURCE-AWARE OPTIMIZATION
     * =================================================
     */

    debug_writeln("");
    debug_writeln("=========================================");
    debug_writeln("[DEMO 6] RESOURCE-AWARE OPTIMIZATION");
    debug_writeln("=========================================");


    /*
     * Reset subsystems before final experiment.
     */
    memory_init();
    process_init();
    scheduler_init();
    resource_init();


    debug_writeln("");
    debug_writeln(
        "[DEMO] Creating HIGH priority Camera..."
    );

    camera_pid =
        process_create(
            "Camera",
            64 * 1024,
            PRIORITY_HIGH
        );


    debug_writeln(
        "[DEMO] Creating MEDIUM priority Sensor..."
    );

    sensor_pid =
        process_create(
            "Sensor",
            32 * 1024,
            PRIORITY_MEDIUM
        );


    debug_writeln(
        "[DEMO] Creating LOW priority Logger..."
    );

    logger_pid =
        process_create(
            "Logger",
            760 * 1024,
            PRIORITY_LOW
        );


    (void)camera_pid;
    (void)sensor_pid;
    (void)logger_pid;


    debug_writeln("");
    debug_writeln(
        "[BEFORE RESOURCE OPTIMIZATION]"
    );

    process_print_table();

    resource_print_status();


    debug_writeln("");
    debug_writeln(
        "[DEMO] EdgeOS checking memory pressure..."
    );

    resource_handle_memory_pressure();


    debug_writeln("");
    debug_writeln(
        "[AFTER RESOURCE OPTIMIZATION]"
    );

    process_print_table();

    resource_print_status();


    debug_writeln("");
    debug_writeln(
        "[DEMO] Scheduling protected workloads..."
    );

    scheduler_run_priority();


    debug_writeln(
        "[PASS] Resource-aware reclamation working."
    );


    /*
     * =================================================
     * FINAL RESULT
     * =================================================
     */

    debug_writeln("");
    debug_writeln("=========================================");
    debug_writeln("       EDGEOS FINAL DEMO COMPLETE");
    debug_writeln("=========================================");

    debug_writeln(
        "[OK] Memory Management"
    );

    debug_writeln(
        "[OK] Fragmentation Optimization"
    );

    debug_writeln(
        "[OK] System Call Dispatcher"
    );

    debug_writeln(
        "[OK] Process Management / PCB"
    );

    debug_writeln(
        "[OK] Priority Scheduling"
    );

    debug_writeln(
        "[OK] Round Robin Scheduling"
    );

    debug_writeln(
        "[OK] Resource-Aware Reclamation"
    );

    debug_writeln("");

    debug_writeln(
        "EdgeOS: Resource-Aware OS for"
    );

    debug_writeln(
        "Memory-Constrained Edge Devices"
    );

    debug_writeln("=========================================");


    while (1)
    {
        __asm__ volatile ("hlt");
    }

}
