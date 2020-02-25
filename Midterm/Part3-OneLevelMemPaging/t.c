int color;

#include "defines.h"            // LCD, TIMER, and UART addresses
#include "type.h"
#include "vid.c"                // LCD Driver
#include "kbd.c"                // Kerboard Driver
#include "string.c"
#include "queue.c"
//#include "uart.c"
#include "exceptions.c"
//#include "pipe.c"
//#include "kernel.c"
//#include "wait.c"
#include "timer.c"

int time_test();                        // TimerQueue Test Program
void timer_handler();
TIMER *tp[4];
//PIPE *kpipe;

/* set up MMU using 1 MB sections to ID map VA to PA */
// Versatilepb: 256 MB RAM + 2 MB I/O sections at 256 MB
/************** L1 section descriptor ****************
    |31            20|19     12|--|9|8765|4|32|10|
    | section addr   |         |AP|0|DOMN|1|CB|10|
    |                |000000000|01|0|0000|1|00|10| = 0x412
    |                           KRW  dom0       |
*****************************************************/


int kprintf(char *fmt, ...);
//int kputc(char);                      //! in pipe.tgz
int body();
int time_test();                        // TimerQueue Test Program
void timer_handler();

int mkptable() // build level-1 pgtable using 1 MB sections
{
    int i, pentry, *ptable;
    printf("1. build level-1 pgtable at 16 KB\n");
    ptable = (int *)0x4000;
    for (i = 0; i < 4096; i++)
    { // zero out the pgtable
        ptable[i] = 0;
    }
    /* printf("2. fill 258 entries of pgtable to ID map 258 MB VA to PA\n");
    pentry = 0x412; // AP = 01,domain = 0000, CB = 00, type = 02 for section
    for (i = 0; i < 258; i++)//TODO: Make change here to 130 MB (128 MB + 2 MB)
    { // 258 level-1 page table entries
        ptable[i] = pentry;
        pentry += 0x100000;
    } */

    printf("2. fill 130 entries of pgtable to ID map 130 MB VA to PA\n");
    pentry = 0x412; // AP = 01,domain = 0000, CB = 00, type = 02 for section
    /* for (i = 0; i < 130; i++)//* 130 MB (128 MB + 2 MB I/O)
    { // 130 level-1 page table entries
        if ( i < 130)
        {
            ptable[i] = pentry;
        }
        pentry += 0x100000;
    } */

    for (i = 0; i < 130; i++)//* 130 MB (128 MB + 2 MB I/O)
    { // 130 level-1 page table entries    
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

//void copy_vectors(void)
int copy_vector_table()
{
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while (vectors_src < &vectors_end)
        *vectors_dst++ = *vectors_src++;
}

void irq_chandler()
{
    // read VIC status registers to find out which interrupt
    int vicstatus, sicstatus;
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;

    // VIC status BITs: timer0,1=4, uart0=13, uart1=14
    if (vicstatus & (1 << 4))
    {                     // bit4=1:timer0,1
        timer_handler(0); // timer0 only
    }

/*     if (vicstatus & (1 << 12))
    {
        uart0_handler();
    }
     */

    if (vicstatus & (1 << 31))
    {
        if (sicstatus & (1 << 3))
        {
            kbd_handler();
            //kfork((int)kbd_handler, 1); // It's own process to go to sleep
        }
    }
}


int main()
{
    int i, *p;
    char line[128];

    color = WHITE;

    //fbuf_init();

    kbd_init();
    //uart_init();


    // allow KBD interrupts
    VIC_INTENABLE = 0;
    VIC_INTENABLE |= (1 << 12); // UART0 at 12
    VIC_INTENABLE |= (1 << 31); // allow VIC IRQ31

    UART0_IMSC = 1 << 4;         // enable UART RX interrupts

    // enable KBD IRQ
    SIC_INTENABLE = (1<<3); // KBD int=bit3 on SIC //!This one is from pipe.tgz
    SIC_ENSET = 1 << 3;    // KBD int=3 on SIC
    SIC_PICENSET = 1 << 3; // KBD int=3 on SIC

    // enable VIC for timer interrupts
    VIC_INTENABLE |= (1<<4); //timer0,1 at VIC.bit 4
    
    timer_init();
    tp[0] = &timer[0];
    timer_start(0);

    printf("test MMU rpotection: try to access VA =  0x00200000\n");
    p = (int *)0x002000000;
    *p = 123;
    printf("test MMU protection: try to access VA = 0x02000000\n");
    p = (int *)0x020000000;
    *p = 123;
    printf("test MMU protection: try to access VA = 0x20000000\n");
    p = (int *)0x20000000;
    *p = 123;

    
    /* printf("test MM at VA=2MB\n");
    p = (int *)(2 * 0x100000);
    *p = 123;
    
    printf("test MM at VA=2MB\n");
    p = (int *)(127 * 0x100000);
    *p = 123;

    printf("test MM at VA=128MB\n");
    p = (int *)(128 * 0x100000);
    *p = 123;

    printf("test MM at VA=128MB\n");
    p = (int *)(512 * 0x100000);
    *p = 123; */
    
   color=PURPLE;
    while(1)                                //! Probably not due to kforks above
    {
        color=PURPLE;
        printf("main running Input a line: ");
        kgets(line);
        printf(" line = %s\n", line);
    }
}
