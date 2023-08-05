[org 0x7e00]
jmp protectedmode

%include "src/kernel/gdt.asm"   ; Make sure its the exact location of the GDT file

protectedmode:
    call EnableA20
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp codeseg:StartProtectedMode

EnableA20: ;using FAST A20
    in al, 0x92
    or al, 2
    out 0x92, al
    ret
[bits 32]
[extern _main]
StartProtectedMode:

    mov [0xb8000], byte 'X'
    jmp _main
    ; We can now access up to 0xFFFFFFF0 memory (~4GB!) instead of 1MB (0xFFFFF)
    jmp $


times 2048-($-$$) db 0 ; allocate ≈2kb for the kernel
; Why this works is because memory from 0x7e00 -- 0x9fc00 is free (~638KB)
; and i think that GDT is mapped to the 2KB area and if we load more GDT will
; freak out and triple fault the CPU.