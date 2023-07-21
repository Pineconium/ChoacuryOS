; This is just a placeholder kernel okay...
org 0
bits 16

section .text
    global _start

_start:
    mov     ah, 0x0e
    mov     al, "A"     ; This some reason makes a lot of weird text
    mov     bh, 0x00
    int     0x10
    jmp     .done

.done:
    hlt

times (512-($-$$)) db 0
dw 0xaa55
