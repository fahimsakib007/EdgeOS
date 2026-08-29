#ifndef MEMORY_H
#define MEMORY_H

void memory_init(void);

void* kmalloc(unsigned int size);

void kfree(void* ptr);

void memory_print(void);


/*
 * Resource monitoring interface
 */
unsigned int memory_get_used(void);

unsigned int memory_get_free(void);

unsigned int memory_get_fragmentation(void);


#endif
