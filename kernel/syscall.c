#include "../include/syscall.h"
#include "../include/memory.h"
#include "../include/debug.h"


unsigned int syscall_dispatch(
    unsigned int syscall_number,
    unsigned int argument
)
{
    /*
     * System Call 1:
     * Request kernel memory
     */
    if (syscall_number == SYS_MALLOC)
    {
        debug_writeln(
            "[SYS] SYS_MALLOC received."
        );

        void *ptr = kmalloc(argument);

        if (ptr == 0)
        {
            debug_writeln(
                "[SYS] Memory request failed."
            );

            return 0;
        }

        debug_writeln(
            "[SYS] Memory request successful."
        );

        return (unsigned int)ptr;
    }


    /*
     * System Call 2:
     * Release kernel memory
     */
    if (syscall_number == SYS_FREE)
    {
        debug_writeln(
            "[SYS] SYS_FREE received."
        );

        if (argument != 0)
        {
            kfree((void *)argument);

            debug_writeln(
                "[SYS] Memory released through syscall."
            );
        }

        return 0;
    }


    /*
     * Invalid/unsupported syscall
     */
    debug_writeln(
        "[SYS] Unknown system call."
    );

    return 0;
}
