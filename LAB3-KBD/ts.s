/********* ts.s file ***********/
.text
.code 32

.global reset, vectors_start, vectors_end, reset_handler
.global lock, unlock

reset_handler: // entry point of program
    // set SVC stack
    LDR sp, =svc_stack_top
    // copy vector table to address 0
    BL copy_vectors
    // go in IRQ mode to set IRQ stack
    MSR cpsr, #0x92
    LDR sp, =irq_stack_top
    // go back in SVC mode with IRQ interrupts enabled
    MSR cpsr, #0x13
    // call main() in SVC mode
    BL main
    B .

irq_handler:
    sub lr, lr, #4
    stmfd sp!, {r0-r12, lr} // stack ALL registers
    bl IRQ_handler // call IRQ_hanler() in C
    ldmfd sp!, {r0-r3, r12, pc}^ // return

undef_handler:

swi_handler:

prefetch_abort_handler:

data_abort_handler:

fiq_handler:

lock: // mask out IRQ interrupts
    MRS r0, cpsr
    ORR r0, r0, #0x80 // set I bit means MASK out IRQ interrupts
    MSR cpsr, r0
    mov pc, lr

unlock: // mask in IRQ interrupts
    MRS r0, cpsr
    BIC r0, r0, #0x80 // clr I bit means MASK in IRQ interrupts
    MSR cpsr, r0
    mov pc, lr



vectors_start:
    LDR PC, reset_handler_addr              //0x00
    LDR PC, undef_handler_addr              //0x04
    LDR PC, swi_handler_addr                //0x08
    LDR PC, prefetch_abort_handler_addr     //0x0C
    LDR PC, data_abort_handler_addr         //0x10
    B .                                     //0x14
    LDR PC, irq_handler_addr                //0x18
    LDR PC, fiq_handler_addr                //0x1C

reset_handler_addr:             .word reset_handler
undef_handler_addr:             .word undef_handler
swi_handler_addr:               .word swi_handler
prefetch_abort_handler_addr:    .word prefetch_abort_handler
data_abort_handler_addr:        .word data_abort_handler
irq_handler_addr:               .word irq_handler
fiq_handler_addr:               .word fiq_handler

vectors_end:
