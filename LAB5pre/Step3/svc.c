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
//#include "type.h"
extern PROC proc[], *running;
extern int tswitch();

int ktswitch()
{
    // switch process
    tswitch();
}

int kgetpid()
{
    //return pid;
    return running->pid;
}

int kgetppid()
{
    //return ppid;
    return running->ppid;
}


char *pstatus[] = {"FREE   ", "READY  ", "SLEEP  ", "BLOCK  ", "ZOMBIE", " RUN  "};

int kps()
{
    int i = 0;
    int tempColor = 0;
    PROC *p = 0;

    // print process info
    /* printf("\nPS:\n");
    PROC *p = running;
    while(p){
        printf("Process %d | Name = %s |Status = ", p->pid, p->name);
        tempColor=color;
        color=PURPLE;
        printf("%s\n",  pstatus[p->status]);
        color=tempColor;
        p = p->next;
        if(p->pid == 0)
        {
            break;
        }
    } */

    printf("\nPS:\n");
    do {
        p = &proc[i];
        printf("Process %d | Name = %s |Status = ", p->pid, p->name);
        tempColor=color;
        color=PURPLE;
        printf("%s\n",  pstatus[p->status]);
        color=tempColor;
        p = p->next;
        i++;
    } while (i < NPROC);


    printf("\n");

    return 1;
}


int kchname(char *s)
{
    // change process name  to string s
    strcpy(running->name, s);
    return 1;
}

int kgetPA()
{
    //return Umode PA of process
    //return &ptable[2058];
    //u32 *ut = (u32 *)0x400000;   // at 4MB
    //printf("\n\nPA: Hex:%x  Int:%d\n\n", &(ut[2048]), &(ut[2048]));
    //return &(ut[2048]); //For testing only

    printf("\n\nPA: Hex:%x  Int:%d\n\n", running->pgdir[2048], running->pgdir[2048]);
    return running->pgdir[2048];
    //TODO: I think I got to start a pgdir on the proc
}

int svc_handler(int a, int b, int c, int d)
{
    int r;

    // printf("svc_handler: a=%d b=%d c=%d d=%d\n",a,b,c,d);

    switch (a)
    { // a is the call number

    case 0:
        r = kgetpid();
        break;
    case 1:
        r = kgetppid();
        break;
    case 2:
        r = kps();
        break;
    case 3:
        r = kchname((char *)b);
        break;
    case 4:
        r = ktswitch();
        break;

    case 90:
        r = kgetc();
        break;
    case 91:
        r = kputc(b);
        break;
    case 92:
        r = kgetPA();
        break;
    default:
        printf("invalid syscall %d\n", a);
    }

    return r; //return to goUmode in ts.s, which will replace r0 in Kstack with r
}
