mov ah, 0x0e
mov al, 'C'
int 0x10
mov al, 'h'
int 0x10
mov al, 'o'
int 0x10
mov al, 'a'
int 0x10
mov al, 'K'
int 0x10
mov al, 'e'
int 0x10
mov al, 'r'
int 0x10
mov al, 'n'
int 0x10
times 16192-($-$$) db 0 ; allocate ≈16kb for the kernel
