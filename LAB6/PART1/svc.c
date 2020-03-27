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

char *pstatus[] = {"FREE   ", "READY  ", "SLEEP  ", "BLOCK  ", "ZOMBIE", " RUNNING  "};

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
    do
    {
        p = &proc[i];

        printf("PROC[ %d]: PID=%d | PPID=%d | Name = %s |\t Status = ", i, p->pid, p->ppid, p->name);
        tempColor = color;
        color = PURPLE;
        if (p == running)
        {
            printf("%s\n", pstatus[5]);
        }
        else
        {
            printf("%s\n", pstatus[p->status]);
        }
        color = tempColor;
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

    printf("\nPA: Hex:%x  Int:%d\n", running->pgdir[2048], running->pgdir[2048]);
    return running->pgdir[2048];
    //TODO: I think I got to start a pgdir on the proc
}

int fork()
{
    int i;
    char *PA, *CA;
    //PROC *p = get_proc(&freeList);
    PROC *p = getproc();

    if (p == 0)
    {
        printf("fork failed\n");
        return -1;
    }

    p->ppid = running->pid;
    p->parent = running;
    p->status = READY;
    p->priority = 1;

    uPtable(p);
    
    printf("running usp=%x | upc=%x\n", running->usp, running->upc);

    //PA = (char *)running->pgdir[2048] & 0xFFFF0000; // parent Umode PA
    //CA = (char *)p->pgdir[2048] & 0xFFFF0000;       // child Umode PA
    PA =  running->pgdir[2048] & 0xFFFF0000;        // parent Umode PA
    CA =  p->pgdir[2048] & 0xFFFF0000;              // child Umode PA
    printf("FORK: child %d uimage at %x\n", p->pid, CA);
    printf("copy Umode image from %x to %x\n", PA, CA);

    memcpy(CA, PA, 0x100000);                       // copy 1MB Umode image

    // The hard part: child must resume to the same place as parent
    // Child kstack must contain | parent kstack | goUmode stack | => copy kstack
    printf("Copy kernel mode stack\n");

    for (i = 1; i <= 14; i++)
    { // copy bottom 14 entries of kstack
        p->kstack[SSIZE - i] = running->kstack[SSIZE - i];
    }

    for(i=15; i <= 28; i++)
    {
        p->kstack[SSIZE - i] = 0;
    }

    p->kstack[SSIZE - 14] = 0;            // child return pid = 0
    p->kstack[SSIZE - 15] = (int)goUmode; // child resumes to goUmode
    p->ksp = &(p->kstack[SSIZE - 28]);    // child saved ksp
    p->usp = running->usp;                // same usp as parent
    p->cpsr = running->cpsr;              // same spsr as parent
    p->upc = running->upc;

    enqueue(&readyQueue, p);
    printf("KERNEL: proc %d forked a child %d\n", running->pid, p->pid);
    printQ(readyQueue);

    return p->pid;
}

int exec(char *cmdline) // cmdline=VA in Uspace
{
    int i, upa, usp;
    char *cp, kline[128], file[32], filename[32];
    PROC *p = running;
    int umodeSize = 0x100000;

    printf("EXEC: proc %d cmdline=%x\n", running->pid, cmdline);

    
    strcpy(kline, cmdline); // fetch cmdline into kernel space
    printf("EXEC: proc %d kline = %s\n", running->pid, kline);

    // get first token of kline as filename
    cp = kline;
    i = 0;
    while (*cp != ' ')
    {
        filename[i] = *cp;
        i++;
        cp++;
    }

    filename[i] = 0;
    file[0] = 0;
    if (filename[0] != '/')    // if filename relative
    {
        strcpy(file, "/bin/"); // prefix with /bin/
    }
    //strcat(file, filename);
    kstrcat(file, filename);
    
    upa = p->pgdir[2048] & 0xFFFF0000; // PA of Umode image
    // loader return 0 if file non-exist or non-executable
    kprintf("load file %s to %x\n", file, upa);
    
    //if (!loadelf(file, p))
    if (!load(file, p))
    {
        printf("Loading FAILED!\n");
        return -1;
    }

    // copy cmdline to high end of Ustack in Umode image
    usp = upa + 0x100000 - 128; // assume cmdline len < 128
    printf("usp=%x ", usp);
    strcpy((char *)usp, kline);

    p->usp = (int *)VA(0x100000 - 128);
    printf("p->usp = %x ", p->usp);
    // fix syscall frame in kstack to return to VA=0 of new image
    
    for (i = 2; i < 14; i++) // clear Umode regs r1-r12
    {
        p->kstack[SSIZE - i] = 0;
    }
    
    p->kstack[SSIZE - 14] = (int)VA(p->usp);
    p->kstack[SSIZE - 1] = (int)VA(0); // return uLR = VA(0)

    kprintf("kexec exit\n");
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
    case 5:
        r = ksleep((int)b);
        break;
    case 6:
        r = kwakeup((int)b);
        break;
    case 7:
        r = kfork((char *)b);
        break;
    case 8:
        r = kexit((int)b);
        break;
    case 9:
        r = kwait((int *)b);
        break;
    case 10:
        r = fork();
        break;
    case 11:
        r = exec((char *)b);
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
