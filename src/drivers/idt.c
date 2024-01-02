#include "idt.h"
#include "gdt.h"
#include "utils.h"
#include "vga.h"

/* Macros for interrupt code gen */
#define ISR_LIST_X X(0) X(1) X(2) X(3) X(4) X(5) X(6) X(7) X(8) X(9) X(10) X(11) X(12) X(13) X(14) X(15) X(16) X(17) X(18) X(19) X(20) X(21) X(22) X(23) X(24) X(25) X(26) X(27) X(28) X(29) X(30) X(31)
#define IRQ_LIST_X X(0) X(1) X(2) X(3) X(4) X(5) X(6) X(7) X(8) X(9) X(10) X(11) X(12) X(13) X(14) X(15) X(16) X(17) X(18) X(19) X(20) X(21) X(22) X(23) X(24) X(25) X(26) X(27) X(28) X(29) X(30) X(31)

/* How every interrupt gate (handler) is defined */
typedef struct {
    u16 low_offset; // <-- Lower 16 bits of handler function address
    u16 sel;        // <-- Kernel segment selector
    u8 always0;
    /* First byte
     * Bit 7: "Interrupt is present"
     * Bits 6-5: Privilege level of caller (0=kernel..3=user)
     * Bit 4: Set to 0 for interrupt gates
     * Bits 3-0: bits 1110 = decimal 14 = "32 bit interrupt gate" */
    u8 flags; 
    u16 high_offset; // <-- Higher 16 bits of handler function address
} __attribute__((packed)) idt_gate_t ;

/* A pointer to the array of interrupt handlers.
 * Assembly instruction 'lidt' will read it */
typedef struct {
    u16 limit;
    void* base;
} __attribute__((packed)) idt_register_t;

#define IDT_ENTRIES 256
static idt_gate_t idt[IDT_ENTRIES];
static idt_register_t idt_reg;

static irq_handler_t irq_handlers[32];

static const char* isr_names[] = {
    "Division Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint ",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved 22",
    "Reserved 23",
    "Reserved 24",
    "Reserved 25",
    "Reserved 26",
    "Reserved 27",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved 31"
};

void c_isr_handler(u8 isr, u32 error) {
    k_printf(isr_names[isr], 0, TC_LRED);
    for (;;) {
        asm volatile("hlt");
    }
}

void c_irq_handler(u8 irq) {
    irq_handler_t handler = irq_handlers[irq];
    if (handler == 0) {
        k_printf("no handler for irq", 0, TC_YELLO);
        return;
    }

    handler();
}

static void set_idt_gate(int n, void (*handler)()) {
    idt[n].low_offset = ((uptr)handler >> 0) & 0xFFFF;
    idt[n].sel = KERNEL_CS;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E; 
    idt[n].high_offset = ((uptr)handler >> 16) & 0xFFFF;
}

void idt_register_irq_handler(int irq, void (*handler)()) {
    irq_handlers[irq] = handler;
}

static void idt_flush() {
    asm volatile("lidt %0" :: "m"(idt_reg));
}

/* Code gen externs for isr handler (defined in interrupt.asm) */
#define X(num) extern void isr ## num();
    ISR_LIST_X
#undef X

/* Code gen externs for irq handler (defined in interrupt.asm) */
#define X(num) extern void irq ## num();
    IRQ_LIST_X
#undef X

void idt_init() {
    memory_set((u8*)idt, 0, sizeof(idt));
    memory_set((u8*)irq_handlers, 0, sizeof(irq_handlers));

    idt_reg.base = idt;
    idt_reg.limit = sizeof(idt) - 1;

    /* Code gen isr handler registration */
#define X(num) set_idt_gate(num, isr ## num);
    ISR_LIST_X
#undef X

    /* Code gen irq handler registration */
#define X(num) set_idt_gate(num + IRQ_BASE, irq ## num);
    ISR_LIST_X
#undef X

    idt_flush();
}
