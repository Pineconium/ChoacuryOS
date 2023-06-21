section .text
    ; Bootloader entry point
    ; ...

    ; Load kernel code
    mov ah, 0x02    ; BIOS read sector function
    mov al, 1       ; Number of sectors to read
    mov ch, 0       ; Cylinder number
    mov dh, 0       ; Head number
    mov cl, 2       ; Sector number
    mov bx, 0x1000  ; Destination memory address to load the kernel
    int 0x13        ; Call BIOS interrupt to read the sector

    ; Set up stack
    mov ax, 0x1000  ; Set stack segment
    mov ss, ax
    mov sp, 0       ; Set stack pointer

    ; Jump to kernel entry point
    jmp 0x1000:0    ; Jump to the start of the loaded kernel code
