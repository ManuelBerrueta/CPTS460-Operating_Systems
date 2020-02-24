/************** t.c file of Program C6.1 **************/
int color;

#include "defines.h"            // LCD, TIMER, and UART addresses
#include "type.h"
#include "string.c"
#include "queue.c"
#include "vid.c"                // LCD Driver
#include "kbd.c"                // Kerboard Driver
#include "exceptions.c"
#include "pipe.c"
#include "kernel.c"
#include "wait.c"
#include "timer.c"
#include "uart.c"

/* set up MMU using 1 MB sections to ID map VA to PA */
// Versatilepb: 256 MB RAM + 2 MB I/O sections at 256 MB
/************** L1 section descriptor ****************
    |31            20|19     12|--|9|8765|4|32|10|
    | section addr   |         |AP|0|DOMN|1|CB|10|
    |                |000000000|01|0|0000|1|00|10| = 0x412
    |                           KRW  dom0       |
*****************************************************/

int mkptable() // build level-1 pgtable using 1 MB sections
{
    int i, pentry, *ptable;
    printf("1. build level-1 pgtable at 16 KB\n");
    ptable = (int *)0x4000;
    for (i = 0; i < 4096; i++)
    { // zero out the pgtable
        ptable[i] = 0;
    }
    printf("2. fill 258 entries of pgtable to ID map 258 MB VA to PA\n");
    pentry = 0x412; // AP = 01,domain = 0000, CB = 00, type = 02 for section
    for (i = 0; i < 258; i++)
    { // 258 level-1 page table entries
        ptable[i] = pentry;
        pentry += 0x100000;
    }
    printf("3. finished building level-1 page table\n");
    printf("4. return to set TTB, domain and enable MMU\n");
}

int data_chandler() //data abort handler
{
    u32 fault_status, fault_addr, domain, status;
    int spsr = get_spsr();
    printf("data_abort exception in ");
    if ((spsr & 0x1F) == 0x13)
        printf("SVC mode\n");
    fault_status = get_fault_status();
    fault_addr = get_fault_addr();
    domain = (fault_status & 0xF0) >> 4;
    status = fault_status & 0xF;
    printf("status = %x: domain = %x status = %x (0x5 = Trans Invalid)\n",
           fault_status, domain, status);
    printf("VA addr = %x\n", fault_addr);
}

/* int copy_vector_table()
{
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;
    while (vectors_src < &vectors_end)
        *vectors_dst++ = *vectors_src++;
} */

int copy_vector_table()
{
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while (vectors_src < &vectors_end)
        *vectors_dst++ = *vectors_src++;
}


int irq_chandler() // IRQ interrupts handler
{
    // read VIC, SIC status registers to find out which interrupt
    int vicstatus = VIC_STATUS;
    int sicstatus = SIC_STATUS;
    if (vicstatus & (1 << 4))
        timer0_handler();
    if (vicstatus & (1 << 12))
        uart0_handler();
    if (vicstatus & (1 << 31) && sicstatus & (1 << 3))
        kbd_handler();
}

int main()
{
    int i, *p;
    char line[128];
    kbd_init();
    uart_init();
    VIC_INTENABLE |= (1 << 4);  // timer0 at 4
    VIC_INTENABLE |= (1 << 12); // UART0 at 12
    VIC_INTENABLE = 1 << 31;     // SIC to VIC's IRQ31
    UART0_IMSC = 1 << 4;         // enable UART RX interrupts
    SIC_ENSET = 1 << 3;          // KBD int = 3 on SIC
    SIC_PICENSET = 1 << 3;       // KBD int = 3 on SIC
    //kbd->control = 1 << 4;
    timer_init();
    timer_start(0);
    printf("test MMU protection: try to access VA = 0x00200000\n");
    p = (int *)0x002000000;
    *p = 123;
    printf("test MMU protection: try to access VA = 0x02000000\n");
    p = (int *)0x020000000;
    *p = 123;
    printf("test MMU protection: try to access VA = 0x20000000\n");
    p = (int *)0x20000000;
    *p = 123;
    while (1)
    {
        printf("main running Input a line: ");
        kgets(line);
        printf(" line = %s\n", line);
    }
}