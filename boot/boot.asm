section .multiboot
align 4

MAGIC    equ 0x1BADB002
FLAGS    equ 0
CHECKSUM equ -(MAGIC + FLAGS)

dd MAGIC
dd FLAGS
dd CHECKSUM


section .text

global start
extern kernel_main

start:
    cli

    mov esp, stack_top

    call kernel_main

hang:
    hlt
    jmp hang


section .bss

align 16

stack_bottom:
    resb 16384

stack_top:
