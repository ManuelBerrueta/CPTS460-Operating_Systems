// wait.c file

extern PROC *running;
extern PROC *sleepList;



/* // Helper function to verify P1's children
int printP1Children(PROC *P1)
{
    if (P1->child != 0)
    {
        PROC *tempChild = &P1->child;
        
        printf("\n===P1's Children:\n\t");
        while (tempChild != 0)
        {
            printf("Proc %d > ", tempChild->pid);
            tempChild = tempChild->sibling;
        }
        printf(" NULL \n");
    }
} */

int kexit(int exitValue) // SIMPLE kexit() for process to terminate
{
    if(running->pid == 1)
    {
        printf("\n\n}===========================[ CANT KILL P1! ]==========================={\n\n");
        return -1;
    }
    
    
    running->status = ZOMBIE;
    running->exitCode = exitValue;

    // We must check this current proc for children,
    // If it has children, they must become the children of P1
    // Else if no children, we wake up it's parent
    if (running->child)
    {
        treeEnqueue(&proc[1], running->child);
        //printP1Children(&proc[1]);
        printf("P1 Children:\n===>");
        printQueue(&proc[1]);
        running->child = 0;
        kwakeup(&proc[1]);
    }

    printf("Proc %d exit\n", running->pid);
    printf("Proc %d is Zombie Child of Proc %d\n", running->pid, running->parent->pid);
    // Dead child (current running process) must wake up parent, 
    // so parent can take care of dead child.
    kwakeup(running->parent);
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

// Each kwait call handles only one Zombie Child!
int kwait(int *status)
{
    // If calling process has no child
    int zombiePID = -1;
    int zombieExists = 0;
    if (!running->child)
    {
        printf("\n====KWAIT: This Proc has no children!\n");
        return -1;
    }
    
    while (1)
    {
        //! At this point we only know that the current proc has children:
        //! If there exist a child, then there is 3 cases for a Possible zombie Child
        //!     * The running->child is the zombie child
        //!     * One of the siblings is the Zombie child
        //!         * In this case we have to keep track on the previous child
        //!           to be able to link to the next sibling of the zombie child
        //!     * None of them are zombie child

        PROC *possibleZombie = running->child;
        PROC *previousChild = 0; //Have to keep track of prior child
        
        // First case where the running->child is a Zombie
        if(possibleZombie->status == ZOMBIE)
        {
            zombieExists = 1;
        } else {
            
            // Else we look for a possible Zombie Child through siblings
            while(possibleZombie->sibling)
            {
                previousChild = possibleZombie; //Save previous for linking
                possibleZombie = possibleZombie->sibling;
                if (possibleZombie->status == ZOMBIE)
                {
                    zombieExists = 1;
                    break;
                }
            }
        }
        // Now we can check if zombieExists and also if there is a previous child
        // If this is the case then we must link the previous child with the
        // the possibleZombie which is a realy Zombie child's siblings
        if (previousChild && zombieExists)
        {
            previousChild->sibling = possibleZombie->sibling;
            possibleZombie->sibling = 0; //Clear out this proc for next round
        } else if (zombieExists) {
            zombiePID = possibleZombie->pid;
            // We must return the exit code of the zombie child via *status
            *status = possibleZombie->exitCode;
            // We must clear the status for next time it runs
            possibleZombie->status = FREE;
            // Enqueue zombie child back in to the free process list to be used again
            enqueue(&freeList, possibleZombie);
            return(zombiePID);
        }
        ksleep(running);
        //tswitch();
    }
}
