// kernel.c file

extern PROC *getproc();                         //! This is in new queue.c from pipe.tgz
extern PROC *dequeue();                         //! This is in new queue.c from pipe.tgz
//extern int pipe_writer(), pipe_reader();
//extern PIPE *kpipe;                             //! gives error, in t.c need to sort out includes

#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procsize = sizeof(PROC);

int body();


int init()
{
    int i, j;
    PROC *p;
    kprintf("kernel_init()\n");
    for (i = 0; i < NPROC; i++)
    {
        p = &proc[i];
        p->pid = i;
        p->status = READY;
        //p->status = FREE;
        p->next = p + 1;
    }
    p->parent = p;
    p->child = 0;
    p->sibling = 0;
    proc[NPROC - 1].next = 0; // circular proc list

    freeList = &proc[0];
    readyQueue = 0;
    sleepList = 0;

    p = dequeue(&freeList);
    p->priority = 0;
    p->ppid = 0;
    running = p;

    kprintf("running = %d\n", running->pid);
    printList("freeList", freeList);
}


int kfork(int func, int priority)           //! kfork is different in pipe.tgz
{
    int i;
    PROC *p = dequeue(&freeList);
    if (p == 0)
    {
        printf("no more PROC, kfork failed\n");
        return 0;
    }
    p->status = READY;
    p->priority = priority;
    p->ppid = running->pid; //Parent pid
    p->parent = running;
    p->child = 0;
    p->sibling = 0;

    //TODO: if it is p[1] then p0 is not it's child


    //treeEnqueue(p, priority);
    treeEnqueue(running, p);

    // set kstack for new proc to resume to func()
    // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
    //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
    for (i = 1; i < 15; i++)
    {
        p->kstack[SSIZE - i] = 0;
    }
    p->kstack[SSIZE - 1] = (int)func; // in dec reg=address ORDER !!!
    p->ksp = &(p->kstack[SSIZE - 14]);
    
    enqueue(&readyQueue, p);
    
    printf("proc %d kforked a child %d\n", running->pid, p->pid);
    //printList("readyQueue", readyQueue);
    //printList("Proc[%d] Children:", p);
    printChildList(running);

    return p->pid;
}


int scheduler()
{
    //  kprintf("proc %d in scheduler ", running->pid);
    if (running->status == READY)
        enqueue(&readyQueue, running);
    running = dequeue(&readyQueue);
    //kprintf("next running = %d\n", running->pid);
    if (running->pid)
    {
        color = running->pid;
    }
}


int sleepTime()
{
    char inEvent[8];
    int tempEvent = -99;
    
    printf("\n==[Enter Sleep Event Value::> ");
    kgets(inEvent);
    
    tempEvent = atoi(inEvent);
    ksleep(tempEvent);
}


int wakeupTime()
{
    char inEvent[8];
    int tempEvent = -99;

    printf("\n==[Enter Wakeup Event Value::> ");
    kgets(inEvent);

    tempEvent = atoi(inEvent);
    kwakeup(tempEvent);
}

int exitTime()
{
    char inExitStr[16];
    int tempExit = -99;

    printf("\n==[Enter Exit Value::> ");
    kgets(inExitStr);

    tempExit = atoi(inExitStr);
    kexit(tempExit);
}

int waitTime()
{
    int status = -1;
    int pid = kwait(&status);
    if(pid < 0)
    {
        printf("No Zombie Children!\n\n");
    } else {
        printf("\n\n====KWAIT: Proc[%d] Took her of ZombieChild[%d]\n\n", running->pid, pid);
    }
    printChildList(running);
}


int body()
{
    char c, cmd[64];
    int tempEvent = -99;

    kprintf("proc %d resume to body()\n", running->pid);
    while (1)
    {
        printf("-------- proc %d running -----------\n", running->pid);

        printList("freeList  ", freeList);
        printList("readyQueue", readyQueue);
        printsleepList(sleepList);

        printf("Enter a command [sleep|wakeup|switch|kfork|wait|exit] : ",
               running->pid);
        kgets(cmd);

        if (strcmp(cmd, "switch") == 0)
        {
            tswitch();
        }
        else if (strcmp(cmd, "kfork") == 0)
        {
            kfork((int)body, 1);
        }
        else if (strcmp(cmd, "sleep") == 0)
        {
            sleepTime();
        }
        else if (strcmp(cmd, "wakeup") == 0)
        {
            wakeupTime();
        }
        else if (strcmp(cmd, "wait") == 0)
        {
            waitTime();
        }
        else if (strcmp(cmd, "exit") == 0)
        {
            exitTime();
            //exit();
        }
    }
}
