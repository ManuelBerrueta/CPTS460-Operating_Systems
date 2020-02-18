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

int pipe_writer()
{
    char c, *cp;
    struct uart *up = &uart[0];
    char line[128];
    int i;

    while(1)
    {
        uprintf("Enter a line for task1 to get : ");
        ugets(up, line);
        uprints(up, "\r\n");
        printf("proc%d writes %d line=[%s] to pipe %d\n", running->pid, i, line);
        write_pipe(kpipe, line, strlen(line));
    }
    //kpipe->nwriter--;
    //uprintf("pipe writer proc %d exit\n", running->pid);
    //kexit();
}

int pipe_reader()
{
    char c, *cp;
    char line[128];
    int i, j, n;

    for (i = 0; i < 2; i++)
    {
        printf("proc%d read from pipe %d\n", running->pid, i);
        n = read_pipe(kpipe, line, 20);
        printf("proc%d read n=%d bytes from pipe : [", running->pid, n);
        for (j = 0; j < n; j++)
        {
            kputc(line[j]);
        }
        kprintf("]\n");
    }
    printf("pipe reader proc%d exit\n", running->pid);
    kpipe->nreader--;
    kexit();
}


int piping_test()
{
    pipe_init();

    kpipe = create_pipe();


    //! For PIPE TESTING:

    color=PURPLE;
    kprintf("P%d kfork tasks: \n",running->pid);
    color=YELLOW;

    kfork((int)pipe_writer, 1);                //! Uncomment/comment for pipe testing
    kfork((int)pipe_reader, 1);                //! Uncomment/comment for pipe testing
    color=WHITE;
    printQ(readyQueue);                   //! Requires new queue.c from pipe.tgz
    color=WHITE;

    unlock();
    while (1)
    {
        if (readyQueue)
            tswitch();
    }

    //! PIPE Testing Code end - Anything below can be commented out for pipe test
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
    //kfork((int)kbd_init, 1);

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

    //kfork((int)piping_test, 1);

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
