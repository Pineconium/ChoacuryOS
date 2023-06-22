; bootloader.asm
mov ah, 0x0e
mov si, variableName

printString:
    lodsb
    cmp al, 0
    je end
    int 0x10
    jmp printString

end:
    jmp $

variableName:
    db "Choacury Bootloader"

times 510-($-$$) db 0
db 0x55, 0xaa
