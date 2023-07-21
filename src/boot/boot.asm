;
;
; CHOACURY SOURCE CODE FOR THE BOOTLOADER
; Be careful if you are planning to modify this code as this is the bootloader for Choacury
; If you don't know what you are doing then DO NOT MODIFY THIS CODE!
; For help regarding this check the documention file labeled as 'MODDERSGUIDE' in the 'docs' folder.
; Happy modding :-D
;
;

[org 0x7c00]
mov ah, 0x02
    mov bx, 0x7e00
    mov al, 4
    mov dl, 0
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02

int 0x13
jmp 0x7e00

times 510-($-$$) db 0
db 0x55, 0xaa
