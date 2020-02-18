// wait.c file

extern PROC *running;
extern PROC *sleepList;


int kexit() // SIMPLE kexit() for process to terminate
{
    printf("proc %d exit\n", running->pid);
    running->status = ZOMBIE;
    tswitch();
}


//! In sleep() and wakeup() we disable interrupts because each operation must be
//! atomic, that is the operation must be carried out throughout before any
//! other operation, such as an interrupt can be handled.
//! Once the operations are done, we re-enable interrupts

int ksleep(int event)
{
    int SR = int_off();         // Disable IRQ Interrupts and return CPSR
                                // Current Program Status Register (CPSR)
    running->event = event;     // Record the event value in the Proc
    running->status = SLEEP;    // Change Running Proc to SLEEP == 2 status

    printf("\nGo to Sleep PROC[%d] | EVENT: %d\n ", running->pid, running->event);

    enqueue(&sleepList, running);

    printList("SleepList = ", sleepList);

    tswitch();                  // Switch Process

    int_on(SR);                 // Restore original CPSR 
}


int kwakeup(int event)
{
    PROC *eachProc;
    PROC *tempSleepList = 0;

    int SR = int_off();         // Disable IRQ Interrupts and return CPSR
                                // Current Program Status Register (CPSR)
    
    //* Iterate through the sleepList by dequeue, while the pointer is not null.
    //* We check to see if that pointer matches our event. If it matches, then
    //* we change its status to ready and we enqueue it into our readyQueue,
    //* thus waking up the process.
    //* Else we enqueue the proc back to our temporary sleepList.
    //* When we are done iterating through the sleepList, we set the original
    //* sleepList to point to our new tempSleepList.
    
    while (eachProc = dequeue(&sleepList))
    {
        if (eachProc->event == event)
        {
            printf("\nFound PROC[%d] matching EVENT: %d\n\t |-> WOKE UP PROC[%d]\n", eachProc->pid, event, eachProc->pid);
            //printf("Wakeup Proc: %d\n", eachProc->pid);
            eachProc->status = READY;       // READY == 1
            enqueue(&readyQueue, eachProc);
        }
        else
        {
            enqueue(&tempSleepList, eachProc);
        }
    }
    sleepList = tempSleepList;
    
    int_on(SR);

    return 0;
}
