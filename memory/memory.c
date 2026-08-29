#include "../include/memory.h"
#include "../include/debug.h"

#define MEMORY_SIZE (1024 * 1024)
#define MIN_BLOCK_SIZE 4

static unsigned char memory_pool[MEMORY_SIZE];

typedef struct Block
{
    unsigned int size;
    int free;
    struct Block *next;

} Block;

static Block *first_block = 0;


/* Print an unsigned integer to debug console */
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


/*
 * Initialize EdgeOS memory pool
 */
void memory_init(void)
{
    first_block =
        (Block *)memory_pool;

    first_block->size =
        MEMORY_SIZE -
        sizeof(Block);

    first_block->free = 1;

    first_block->next = 0;

    debug_writeln(
        "[MEM] Memory manager initialized."
    );
}


/*
 * Split a large free block.
 *
 * Before:
 *
 * [          FREE BLOCK          ]
 *
 * After:
 *
 * [ USED ][       FREE           ]
 */
static void split_block(
    Block *block,
    unsigned int size
)
{
    unsigned char *new_address;

    new_address =
        (unsigned char *)(block + 1)
        + size;

    Block *new_block =
        (Block *)new_address;

    new_block->size =
        block->size -
        size -
        sizeof(Block);

    new_block->free = 1;

    new_block->next =
        block->next;

    block->size = size;

    block->next =
        new_block;
}


/*
 * Merge adjacent free blocks.
 *
 * Example:
 *
 * [FREE][FREE]
 *
 * becomes
 *
 * [   FREE   ]
 *
 * This reduces external fragmentation.
 */
static void merge_free_blocks(void)
{
    Block *current =
        first_block;

    while (current &&
           current->next)
    {
        if (current->free &&
            current->next->free)
        {
            current->size =
                current->size +
                sizeof(Block) +
                current->next->size;

            current->next =
                current->next->next;

            debug_writeln(
                "[OPT] Adjacent free blocks merged."
            );
        }
        else
        {
            current =
                current->next;
        }
    }
}


/*
 * First-Fit kernel memory allocator
 */
void *kmalloc(unsigned int size)
{
    Block *current =
        first_block;

    if (size == 0)
    {
        debug_writeln(
            "[MEM] Invalid allocation size."
        );

        return 0;
    }


    /*
     * Align allocations to 4 bytes.
     */
    size =
        (size + 3) & ~3;


    while (current)
    {
        if (current->free &&
            current->size >= size)
        {
            /*
             * Split only if enough space
             * remains for another block.
             */
            if (current->size >=
                size +
                sizeof(Block) +
                MIN_BLOCK_SIZE)
            {
                split_block(
                    current,
                    size
                );

                debug_writeln(
                    "[MEM] Free block split."
                );
            }

            current->free = 0;

            debug_writeln(
                "[MEM] Memory allocated."
            );

            return
                (void *)(current + 1);
        }

        current =
            current->next;
    }


    debug_writeln(
        "[MEM] Allocation failed."
    );

    return 0;
}


/*
 * Release previously allocated memory
 */
void kfree(void *ptr)
{
    if (ptr == 0)
    {
        return;
    }

    Block *block =
        (Block *)ptr - 1;

    block->free = 1;

    debug_writeln(
        "[MEM] Memory released."
    );


    /*
     * EdgeOS memory optimization:
     *
     * Whenever a block is released,
     * attempt to combine adjacent
     * free blocks.
     */
    merge_free_blocks();
}


/*
 * Print detailed memory statistics
 */
void memory_print(void)
{
    Block *current =
        first_block;

    unsigned int used_memory = 0;

    unsigned int free_memory = 0;

    unsigned int used_blocks = 0;

    unsigned int free_blocks = 0;

    unsigned int largest_free_block = 0;


    while (current)
    {
        if (current->free)
        {
            free_memory +=
                current->size;

            free_blocks++;

            if (current->size >
                largest_free_block)
            {
                largest_free_block =
                    current->size;
            }
        }
        else
        {
            used_memory +=
                current->size;

            used_blocks++;
        }

        current =
            current->next;
    }


    /*
     * External fragmentation formula:
     *
     * 100 -
     * ((largest free block * 100)
     * / total free memory)
     */
    unsigned int fragmentation = 0;

    if (free_memory > 0)
    {
        fragmentation =
            100 -
            ((largest_free_block * 100)
             / free_memory);
    }


    debug_writeln(
        "---------------------------------"
    );

    debug_writeln(
        "[MEM] Memory Statistics"
    );


    debug_write(
        "[MEM] Used memory: "
    );

    print_uint(
        used_memory
    );

    debug_writeln(
        " bytes"
    );


    debug_write(
        "[MEM] Free memory: "
    );

    print_uint(
        free_memory
    );

    debug_writeln(
        " bytes"
    );


    debug_write(
        "[MEM] Used blocks: "
    );

    print_uint(
        used_blocks
    );

    debug_writeln("");


    debug_write(
        "[MEM] Free blocks: "
    );

    print_uint(
        free_blocks
    );

    debug_writeln("");


    debug_write(
        "[MEM] Largest free block: "
    );

    print_uint(
        largest_free_block
    );

    debug_writeln(
        " bytes"
    );


    debug_write(
        "[MEM] External fragmentation: "
    );

    print_uint(
        fragmentation
    );

    debug_writeln(
        "%"
    );


    debug_writeln(
        "---------------------------------"
    );
}


/*
 * =====================================================
 * RESOURCE MONITOR INTERFACE
 * =====================================================
 *
 * These functions allow other EdgeOS modules
 * to read memory statistics directly.
 */


/*
 * Return total currently allocated memory.
 */
unsigned int memory_get_used(void)
{
    Block *current =
        first_block;

    unsigned int used_memory = 0;


    while (current)
    {
        if (!current->free)
        {
            used_memory +=
                current->size;
        }

        current =
            current->next;
    }


    return used_memory;
}


/*
 * Return total currently available memory.
 */
unsigned int memory_get_free(void)
{
    Block *current =
        first_block;

    unsigned int free_memory = 0;


    while (current)
    {
        if (current->free)
        {
            free_memory +=
                current->size;
        }

        current =
            current->next;
    }


    return free_memory;
}


/*
 * Return external fragmentation percentage.
 *
 * Formula:
 *
 * fragmentation =
 *
 * 100 -
 * ((largest free block * 100)
 * / total free memory)
 */
unsigned int memory_get_fragmentation(void)
{
    Block *current =
        first_block;

    unsigned int free_memory = 0;

    unsigned int largest_free_block = 0;


    while (current)
    {
        if (current->free)
        {
            free_memory +=
                current->size;


            if (current->size >
                largest_free_block)
            {
                largest_free_block =
                    current->size;
            }
        }

        current =
            current->next;
    }


    if (free_memory == 0)
    {
        return 0;
    }


    return
        100 -
        ((largest_free_block * 100)
         / free_memory);
}
