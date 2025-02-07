    .text
.code 32
.global vectors_start, vectors_end
.global reset_handler, mkptable
.global get_fault_status, get_fault_addr, get_spsr

reset_handler:
    LDR sp, =svc_stack_top                              // set SVC stack
    BL fbuf_init                                        // initialize LCD for display
    BL copy_vector_table                                // copy vector table to PA 0

// Setps m1-m4 are to setup the page table
// After this 4 stesp every virtual address (VA) is mapped to a physical address (PA)

// (m1): build level-1 page table using 1MB sections in C code
    BL mkptable

// (m2): set the Translation Table Base resgister to point at the page table, that is set TTB register to 0x4000
    mov r0, #0x4000
    mcr p15, 0, r0, c2, c0, 0                           // set TTB register
    mcr p15, 0, r0, c8, c7, 0                           // flush TLB

// (m3): set the access bits of domain0 to 01=client(check permission). We could set them to 11=master(no check)
    mov r0,#1                                           // 01 for client mode
    mcr p15, 0, r0, c3, c0, 0

// (m4): enable MMU for address translation
    mrc p15, 0, r0, c1, c0, 0                           // get c1 into r0
    orr r0, r0, #0x00000001                             // set bit0 to 1
    mcr p15, 0, r0, c1, c0, 0                           // write to c1
    nop                                                 // time to allow MMU to finish
    nop
    nop
    mrc p15, 0, r2, c2, c0, 0                           // read TLB base reg c2 into r2
    mov r2, r2

// go in ABT mode to set ABT stack
    MSR cpsr, #0x97
    LDR sp, =abt_stack_top

// go in UND mode to set UND stack
    MSR cpsr, #0x9B
    LDR sp, =und_stack_top

// go in IRQ mode to set IRQ stack and enable IRQ interrupts
    MSR cpsr, #0x92                                     // write to cspr, so in IRQ mode now
    LDR sp, =irq_stack_top                              // set IRQ stack poiner

// go back in SVC mode
    MSR cpsr, #0x13                                     // SVC mode with IRQ enabled

// call main() in SVC mode
    BL main
    B .

swi_handler:                                            // dummy swi_handler, not used yet

data_handler:
    sub lr, lr, #4
    stmfd sp!, {r0-r12, lr}
    bl data_chandler                                    // call data_chandler() in C
    ldmfd sp!, {r0-r12, pc}^

irq_handler:
    sub lr, lr, #4
    stmfd sp!, {r0-r12, lr}
    bl irq_chandler
    ldmfd sp!, {r0-r12, pc}^

vectors_start:
    LDR PC, reset_handler_addr
    LDR PC, undef_handler_addr
    LDR PC, swi_handler_addr
    LDR PC, prefetch_abort_handler_addr
    LDR PC, data_abort_handler_addr
    B .
    LDR PC, irq_handler_addr
    LDR PC, fiq_handler_addr

reset_handler_addr:             .word reset_handler
undef_handler_addr:             .word undef_handler
swi_handler_addr:               .word swi_handler
prefetch_abort_handler_addr:    .word prefetch_abort_handler
data_abort_handler_addr:        .word data_handler
irq_handler_addr:               .word irq_handler
fiq_handler_addr:               .word fiq_handler

vectors_end:

get_fault_status:                                       // read and return MMU reg 5
    MRC p15,0,r0,c5,c0,0                                // read DFSR c5
    mov pc, lr

get_fault_addr:                                         // read and return MMU reg 6
    MRC p15,0,r0,c6,c0,0                                // read DFAR
    mov pc, lr

get_spsr:                                               // get SPSR
    mrs r0, spsr
    mov pc, lr


.end