#include "../include/debug.h"

static inline void outb(unsigned short port, unsigned char value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

void debug_write_char(char c)
{
    outb(0xE9, c);
}

void debug_write(const char *text)
{
    int i = 0;

    while (text[i] != '\0')
    {
        debug_write_char(text[i]);
        i++;
    }
}

void debug_writeln(const char *text)
{
    debug_write(text);
    debug_write("\n");
}
