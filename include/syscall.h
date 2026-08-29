#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_MALLOC 1
#define SYS_FREE   2

unsigned int syscall_dispatch(
    unsigned int syscall_number,
    unsigned int argument
);

#endif
