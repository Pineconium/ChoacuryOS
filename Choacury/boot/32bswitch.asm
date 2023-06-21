global _start

section .text
_start:
    jmp gdt_flush

section .data
gdt_descriptor:
    dw 0xFFFF  ; limit (16 bits)
    dw 0x0000  ; base (16 bits)
    db 0x00    ; base (8 bits)
    db 0x9A    ; access byte
    db 0xCF    ; granularity byte
    dd 0x00000000  ; base (32 bits)

section .text
gdt_flush:
    lgdt [gdt_descriptor]  ; load the Global Descriptor Table
    mov eax, cr0
    or eax, 0x00000001     ; set the protected mode bit
    mov cr0, eax
    jmp CODE_SEG:init_pm   ; far jump to protected mode

section .text
CODE_SEG equ 8
[BITS 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000
    call 0x8:protected_mode_code

section .text
[BITS 16]
protected_mode_code:
    mov ah, 0x0E        ; BIOS teletype mode
    mov al, 'I'         ; character to print
    int 0x10            ; invoke BIOS interrupt
    mov al, 'n'         ; character to print
    int 0x10            ; invoke BIOS interrupt
    mov al, ' '
    int 0x10
    mov al, '1'
    int 0x10
    mov al, '6'
    int 0x10
    mov al, '-'
    int 0x10
    mov al, 'b'
    int 0x10
    mov al, 'i'
    int 0x10
    mov al, 't'
    int 0x10
    mov al, ' '
    int 0x10
    mov al, 'm'
    int 0x10
    mov al, 'o'
    int 0x10
    mov al, 'd'
    int 0x10
    mov al, 'e'
    int 0x10
    mov al, 0x0D        ; carriage return
    int 0x10            ; invoke BIOS interrupt
    mov al, 0x0A        ; line feed
    int 0x10            ; invoke BIOS interrupt

    ; Infinite loop to prevent falling through
    cli
hang:
    hlt
    jmp hang

section .data
DATA_SEG equ 16

section .bss
