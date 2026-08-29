.RECIPEPREFIX := >

CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector
LDFLAGS = -m elf_i386 -T linker.ld

OBJS = boot.o \
       kernel.o \
       debug.o \
       memory.o \
       syscall.o \
       process.o \
       scheduler.o \
       resource.o


all: EdgeOS.iso


boot.o: boot/boot.asm
>$(AS) -f elf32 boot/boot.asm -o boot.o


kernel.o: kernel/kernel.c
>$(CC) $(CFLAGS) -c kernel/kernel.c -o kernel.o


debug.o: kernel/debug.c
>$(CC) $(CFLAGS) -c kernel/debug.c -o debug.o


memory.o: memory/memory.c
>$(CC) $(CFLAGS) -c memory/memory.c -o memory.o


syscall.o: kernel/syscall.c
>$(CC) $(CFLAGS) -c kernel/syscall.c -o syscall.o


process.o: kernel/process.c
>$(CC) $(CFLAGS) -c kernel/process.c -o process.o


scheduler.o: kernel/scheduler.c
>$(CC) $(CFLAGS) -c kernel/scheduler.c -o scheduler.o


resource.o: kernel/resource.c
>$(CC) $(CFLAGS) -c kernel/resource.c -o resource.o


kernel.bin: $(OBJS) linker.ld
>$(LD) $(LDFLAGS) -o kernel.bin $(OBJS)
>grub-file --is-x86-multiboot kernel.bin


EdgeOS.iso: kernel.bin
>cp kernel.bin iso/boot/kernel.bin
>grub-mkrescue -o EdgeOS.iso iso


run: EdgeOS.iso
>qemu-system-i386 \
>-cdrom EdgeOS.iso \
>-display none \
>-debugcon stdio \
>-global isa-debugcon.iobase=0xe9


clean:
>rm -f $(OBJS)
>rm -f kernel.bin
>rm -f EdgeOS.iso
>rm -f iso/boot/kernel.bin
