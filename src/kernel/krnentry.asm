bits 32        ;nasm directive
section .text
    ;multiboot spec
    align 4
    dd 0x1BADB002                ;magic
    dd 0x00                      ;flags
    dd - (0x1BADB002 + 0x00)     ;checksum. m+f+c should be zero

global start
extern k_main    ;  <-- The k_main function is defined in kernel.c

start:
    cli  ; stop interrupts

    call k_main

    hlt ; halt the CPU