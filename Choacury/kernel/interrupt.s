handler_rst: bl    table_copy              @ initialise interrupt vector table

             msr   cpsr, #0xD2             @ enter SVC mode with no interrupts
             ldr   sp, =tos_irq            @ initialise SVC mode stack

             msr   cpsr, #0xD3             @ enter SVC mode with no interrupts
             ldr   sp, =tos_svc            @ initialise SVC mode stack

             sub   sp, sp, #68             @ initialise dummy context

             mov   r0, sp                  @ set    C function arg. = SP
             bl    kernel_handler_rst      @ invoke C function

             ldmia sp!, { r0, lr }         @ load   USR mode PC and CPSR
             msr   spsr, r0                @ set    USR mode        CPSR
             ldmia sp, { r0-r12, sp, lr }^ @ load   USR mode registers
             add   sp, sp, #60             @ update SVC mode SP
             movs  pc, lr                  @ return from interrupt

handler_svc: sub   lr, lr, #0              @ correct return address
             sub   sp, sp, #60             @ update SVC mode stack
             stmia sp, { r0-r12, sp, lr }^ @ store  USR registers
             mrs   r0, spsr                @ get    USR        CPSR
             stmdb sp!, { r0, lr }         @ store  USR PC and CPSR

             mov   r0, sp                  @ set    C function arg. = SP
             ldr   r1, [ lr, #-4 ]         @ load                     svc instruction
             bic   r1, r1, #0xFF000000     @ set    C function arg. = svc immediate
             bl    kernel_handler_svc      @ invoke C function

             ldmia sp!, { r0, lr }         @ load   USR mode PC and CPSR
             msr   spsr, r0                @ set    USR mode        CPSR
             ldmia sp, { r0-r12, sp, lr }^ @ load   USR mode registers
             add   sp, sp, #60             @ update SVC mode SP
             movs  pc, lr                  @ return from interrupt

handler_irq: sub   lr, lr, #4              @ correct return address
             sub   sp, sp, #60             @ update IRQ mode stack
             stmia sp, { r0-r12, sp, lr }^ @ store  USR registers
             mrs   r0, spsr                @ get    USR        CPSR
             stmdb sp!, { r0, lr }         @ store  USR PC and CPSR

             mov   r0, sp                  @ set    C function arg. = SP
             bl    kernel_handler_irq      @ invoke C function

             ldmia sp!, { r0, lr }         @ load   USR mode PC and CPSR
             msr   spsr, r0                @ set    USR mode        CPSR
             ldmia sp, { r0-r12, sp, lr }^ @ load   USR mode registers
             add   sp, sp, #60             @ update IRQ mode SP
             movs  pc, lr                  @ return from interrupt

/* The following captures the interrupt vector table, plus a function
 * to copy it into place (which is called on reset): note that
 *
 * - for interrupts we don't handle an infinite loop is realised (to
 *   to approximate halting the processor), and
 * - we copy the table itself, *plus* the associated addresses stored
 *   as static data: this preserves the relative offset between each
 *   ldr instruction and wherever it loads from.
 */

table_data:  ldr   pc, address_rst         @ reset                 vector -> SVC mode
             b     .                       @ undefined instruction vector -> UND mode
             ldr   pc, address_svc         @ supervisor call       vector -> SVC mode
             b     .                       @ abort (prefetch)      vector -> ABT mode
             b     .                       @ abort     (data)      vector -> ABT mode
             b     .                       @ reserved
             ldr   pc, address_irq         @ IRQ                   vector -> IRQ mode
             b     .                       @ FIQ                   vector -> FIQ mode

address_rst: .word handler_rst
address_svc: .word handler_svc
address_irq: .word handler_irq

table_copy:  mov   r0, #0                  @ set destination address
             ldr   r1, =table_data         @ set source      address
             ldr   r2, =table_copy         @ set source      limit

table_loop:  ldr   r3, [ r1 ], #4          @ load  word, inc. source      address
             str   r3, [ r0 ], #4          @ store word, inc. destination address

             cmp   r1, r2
             bne   table_loop              @ loop if address != limit

             mov   pc, lr                  @ return

/* These function enable and disable IRQ interrupts respectively, by
 * toggling the 7-th bit of CPSR to either 0 or 1.
 */

.global irq_enable
.global irq_unable

irq_enable:  mrs   r0,   cpsr              @  enable IRQ interrupts
             bic   r0, r0, #0x80
             msr   cpsr_c, r0

             mov   pc, lr

irq_unable:  mrs   r0,   cpsr              @ disable IRQ interrupts
             orr   r0, r0, #0x80
             msr   cpsr_c, r0

             mov   pc, lr
