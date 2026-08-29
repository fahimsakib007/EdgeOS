#ifndef RESOURCE_H
#define RESOURCE_H


#define MEMORY_PRESSURE_NORMAL    0
#define MEMORY_PRESSURE_WARNING   1
#define MEMORY_PRESSURE_CRITICAL  2


void resource_init(void);

unsigned int resource_get_memory_pressure(void);

void resource_print_status(void);

void resource_handle_memory_pressure(void);


#endif
