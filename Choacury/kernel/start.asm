section .text
    global _start

_start:
    ; Set up the stack
    mov esp, stack

    ; Call the kernel initialization function
    call kernel_init

    ; Loop indefinitely
    hang:
        jmp hang

section .bss
    stack resb 4096  ; 4KB stack space

section .data
    ; Kernel initialization function
    kernel_init:
        ; Your initialization code here

        ; Return from the kernel initialization function
        ret

