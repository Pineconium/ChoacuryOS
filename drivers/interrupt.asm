bits 32
section .text

extern c_isr_handler
extern c_irq_handler

isr_stub:
    ; Save all registers
    pushad

    ; Push C arguments to stack
    mov eax, [esp + 8*4] ; <- ISR number
    mov ebx, [esp + 9*4] ; <- Error code
    push ebx
    push eax

    ; Calls the C ISR Handler, I mean its pretty obvious by that name
    call c_isr_handler

    ; skip pushed arguments from stack and restore registers
    add esp, 8
    popad
    add esp, 8

    iret

irq_stub:
    ; Save all registers
    pushad

    ; Push IRQ number (a.k.a. A C argument) to stack
    mov eax, [esp + 8*4]
    push eax

    ; Calls the C IRQ Handler
    call c_irq_handler

    ; skip pushed argument from stack and restore registers
    add esp, 4
    popad
    add esp, 8

    iret

%macro isr_gen 1
    global isr %+ %1
    isr %+ %1:
        push 0x69
        push %1
        jmp isr_stub
%endmacro

%macro isr_err_gen 1
    global isr %+ %1
    isr %+ %1:
        push %1
        jmp isr_stub
%endmacro

%macro irq_gen 1
    global irq %+ %1
    irq %+ %1:
        push 0
        push %1
        jmp irq_stub
%endmacro

isr_gen 0
isr_gen 1
isr_gen 2
isr_gen 3
isr_gen 4
isr_gen 5
isr_gen 6
isr_gen 7
isr_err_gen 8
isr_gen 9
isr_err_gen 10
isr_err_gen 11
isr_err_gen 12
isr_err_gen 13
isr_err_gen 14
isr_gen 15
isr_gen 16
isr_err_gen 17
isr_gen 18
isr_gen 19
isr_gen 20
isr_err_gen 21
isr_gen 22
isr_gen 23
isr_gen 24
isr_gen 25
isr_gen 26
isr_gen 27
isr_gen 28
isr_err_gen 29
isr_err_gen 30
isr_gen 31

irq_gen 0
irq_gen 1
irq_gen 2
irq_gen 3
irq_gen 4
irq_gen 5
irq_gen 6
irq_gen 7
irq_gen 8
irq_gen 9
irq_gen 10
irq_gen 11
irq_gen 12
irq_gen 13
irq_gen 14
irq_gen 15
irq_gen 16
irq_gen 17
irq_gen 18
irq_gen 19
irq_gen 20
irq_gen 21
irq_gen 22
irq_gen 23
irq_gen 24
irq_gen 25
irq_gen 26
irq_gen 27
irq_gen 28
irq_gen 29
irq_gen 30
irq_gen 31
