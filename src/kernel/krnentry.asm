bits 32        ;nasm directive

section .multiboot
    ;multiboot spec
    align 4
    dd 0x1BADB002                ;magic
    dd 0x00                      ;flags
    dd - (0x1BADB002 + 0x00)     ;checksum. m+f+c should be zero

section .bss
    boot_stack_bottom:
        resb 4096
    boot_stack_top:

section .data
    boot_gdt:
        dq 0x0000000000000000
        dq 0x00CF9A000000FFFF
        dq 0x00CF92000000FFFF
    boot_gdtr:
        dw boot_gdtr - boot_gdt - 1
        dd boot_gdt

section .text

global start
extern k_main    ;  <-- The k_main function is defined in kernel.c

start:
    mov esp, boot_stack_top

    lgdt [boot_gdtr]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:flush_cs
flush_cs:

    call k_main  ; <-- Jumps to the kernel

    hlt ; <-- Halt the CPU
