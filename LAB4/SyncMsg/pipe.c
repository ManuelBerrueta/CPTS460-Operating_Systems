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

#define NPIPE 10
#define PSIZE 8

typedef struct pipe
{
    char buf[PSIZE];      // Circular data buffer
    int head, tail;       // Circular buffer index
    int data, room;       // Number of data & room in pipe
    int nreader, nwriter; //
    int busy;             // Free or Busy Status
} PIPE;

PIPE pipe[NPIPE];
extern PROC *running;

int pipe_init()
{
    int i;
    printf("pipe_init()\n");
    for (i = 0; i < NPIPE; i++)
    {
        pipe[i].busy = 0;
        pipe[i].nwriter = 0;
        pipe[i].nreader = 0;
    }
}

//Create pipe object in shared address space of all processes
PIPE *create_pipe() //Allocate a free PIPE, initizze and turen a pointer to it.
{
    int i;
    PIPE *p;
    printf("creating pipe ");
    for (i = 0; i < NPIPE; i++)
    {
        p = &pipe[i];
        if (p->busy == 0)
        {
            p->busy = 1;
            break;
        }
    }
    p->head = p->tail = 0;
    p->data = 0;
    p->room = PSIZE;

    printf("OK\n");
    return p;
}

// Read n bytes from pipe. Returns actual number of bytes read from pipe.
int read_pipe(PIPE *p, char *buf, int n)
{
    int r = 0;

    if (n <= 0) // || p->nwriter <=0) //Check for no writer
    {
        p->nreader--; // decrement this reader that was created
        //kwakeup((int)&p->room);
        kwakeup((int)&p->data);
        kexit(running->pid); //kill this process
        return -1;
    }

    while (n)
    {
        while (p->data)
        {
            *buf = p->buf[p->tail++];
            p->tail %= PSIZE;
            p->data--;
            p->room++;
            buf++;
            r++;
            n--;
            if (n == 0)
                break;
        }

        kwakeup((int)&p->room);

        if (r)
        {
            return r;
        }

/*         if(p->nwriter > 0)
        {
            //printf("Broken PIPE!\n");
            kwakeup((int)&p->room);
            ksleep((int)&p->data);  
            return 0;

        } */

        if(!p->nwriter)
        {
            return -1;
        }

        //kwakeup((int)&p->room);
        ksleep((int)&p->data);
    }
}

// Write n bytes from the *buf to the pipe; return # bytes written to pipe.
int write_pipe(PIPE *p, char *buf, int n)
{
    int r = 0;
    int w = 0;

    if (p->nreader <= 0)
    {
        p->nwriter--; //Decrement writer
        printf("Broken PIPE!\n");
        kexit(running->pid); //Exit with the pid number of the writer
        return -1;
    }


    while (n)
    {
        w = 0;
        while (p->room)
        {
            p->buf[p->head++] = *buf;
            p->head %= PSIZE;
            p->data++;
            p->room--;
            buf++;
            w++;
            r++;
            n--;
            if (n == 0)
                break;
        }
        printf("proc%d wrote %d bytes\n", running->pid, w);
        kwakeup((int)&p->data);
        if (n == 0)
        {
            printf("proc%d finished writing %d bytes\n", running->pid, r);
            kwakeup((int)&p->data);
            return r;
        }
        
        
        ksleep((int)&p->room);
    }
}
