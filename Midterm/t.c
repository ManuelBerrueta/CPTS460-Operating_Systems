/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
int color;

#include "defines.h"
#include "type.h"
#include "string.c"
#include "queue.c"
#include "vid.c"
#include "kbd.c"
#include "exceptions.c"
#include "pipe.c"
#include "kernel.c"
#include "wait.c"
#include "uart.c"

//char *tab = "0123456789ABCDEF";

PIPE *kpipe;

int kprintf(char *fmt, ...);
//int kputc(char);                      //! in pipe.tgz
int body();

void copy_vectors(void)
{
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while (vectors_src < &vectors_end)
        *vectors_dst++ = *vectors_src++;
}

void IRQ_handler()
{
    int vicstatus, sicstatus;
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;

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
    int i;
    char line[128];
    u8 kbdstatus, key, scode;
    KBD *kp = &kbd;
    color = WHITE;
    row = col = 0;

    fbuf_init();

    color=GREEN;
    kprintf("Welcome to BERRNIX in Arm\n");
    color = WHITE;

    kbd_init();

    // allow KBD interrupts
    VIC_INTENABLE = 0;
    VIC_INTENABLE |= (1 << 31); // allow VIC IRQ31

    // enable KBD IRQ
    SIC_INTENABLE = (1<<3); // KBD int=bit3 on SIC //!This one is from pipe.tgz
    SIC_ENSET = 1 << 3;    // KBD int=3 on SIC
    SIC_PICENSET = 1 << 3; // KBD int=3 on SIC

    *(kp->base + KCNTL) = 0x12;                     //!From pipe.tgz

    init();

    uart_init();

    color=CYAN;
    kfork((int)body, 1);


    color = WHITE;

    unlock();
    color=RED;
    printf("P%d switch to P1\n", running->pid);
    color=WHITE;
    while(1)                                //! Probably not due to kforks above
    {
        if (readyQueue)
        {

            tswitch();
        }
    }
}
