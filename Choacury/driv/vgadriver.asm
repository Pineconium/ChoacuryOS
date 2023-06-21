[bits 16]

; VGA ports
VGA_PORT_INDEX equ 0x3D4
VGA_PORT_DATA equ 0x3D5

; VGA control registers
VGA_CTRL_INDEX_CURSOR_START_LOW equ 0x0A
VGA_CTRL_INDEX_CURSOR_START_HIGH equ 0x0B
VGA_CTRL_INDEX_CURSOR_END_LOW equ 0x0C
VGA_CTRL_INDEX_CURSOR_END_HIGH equ 0x0D

; VGA text mode dimensions
VGA_TEXT_MODE_WIDTH equ 80
VGA_TEXT_MODE_HEIGHT equ 25

section .text
    global vgadriver

vgadriver:
    pusha

    ; Set cursor position to (0, 0)
    mov al, VGA_CTRL_INDEX_CURSOR_START_LOW
    mov dx, VGA_PORT_INDEX
    out dx, al
    inc dx
    xor ah, ah
    mov al, 0
    mov ah, VGA_CTRL_INDEX_CURSOR_START_HIGH
    out dx, ah

    ; Clear the screen
    xor ax, ax
    xor di, di
    mov cx, VGA_TEXT_MODE_WIDTH * VGA_TEXT_MODE_HEIGHT
    rep stosw

    popa
    ret
