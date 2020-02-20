// kernel.c file

extern PROC *getproc();                         //! This is in new queue.c from pipe.tgz
extern PROC *dequeue();                         //! This is in new queue.c from pipe.tgz
extern int pipe_writer(), pipe_reader();
extern PIPE *kpipe;                             //! gives error, in t.c need to sort out includes

#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procsize = sizeof(PROC);

//For message.c
#define NMBUF 10
struct semaphore nmbuf, mlock;
MBUF mbuf[NMBUF], *mbufList, *mreadyQueue;         // mbufs buffers and mbufList

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
        p->next = p + 1;
        p->mQueue = 0;
        p->mQlock.value = 1;
        p->mQlock.queue = 0;
        p->nmsg.value = 0;
        p->nmsg.queue = 0;
    }
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

int semaphore_init()
{
    int i, j;
    MBUF *p;
    kprintf("semaphore_init()\n");
    for (i = 0; i < NMBUF; i++)
    {
        p = &mbuf[i];
        p->pid = i;
        p->priority = READY;
        //p->contents = "\0";
        p->next = p + 1;
    }
    mbuf[NMBUF - 1].next = 0; // circular proc list

    //freeList = &proc[0];
    mreadyQueue = 0;
    //sleepList = 0;

    //p = dequeue(&freeList);
    //p->priority = 0;
    //p->ppid = 0;
    //running = p;

    //kprintf("running = %d\n", running->pid);
    //printList("freeList", freeList);
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
    p->ppid = running->pid;
    p->parent = running;

    // set kstack for new proc to resume to func()
    // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
    //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
    for (i = 1; i < 15; i++)
    {
        p->kstack[SSIZE - i] = 0;
    }
    p->kstack[SSIZE - 1] = (int)func; // in dec reg=address ORDER !!!
    p->ksp = &(p->kstack[SSIZE - 14]);
    
    if (func == pipe_writer)
    {
        kpipe->nwriter++;
    }
    if (func == pipe_reader)
    {
        kpipe->nreader++;
    }
    
    enqueue(&readyQueue, p);
    
    printf("proc %d kforked a child %d\n", running->pid, p->pid);
    printList("readyQueue", readyQueue);
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

        printf("Enter a command [sleep|wakeup|switch|kfork|exit] : ",
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
        else if (strcmp(cmd, "exit") == 0)
        {
            kexit();
        }
    }
}
