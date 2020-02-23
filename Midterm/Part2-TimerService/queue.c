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

// queue.c file
//extern PROC *freeList;
PROC *getproc(PROC **list)
{
    PROC *p = *list;
    if (p)
    {
        *list = p->next;
    }
    return p;
}

int putproc(PROC **list, PROC *p)
{
    p->next = *list;
    *list = p;
}

treeEnqueue(PROC *parent, PROC *inChild)
{
    // We must check first if the parent has a child
    // If the parents has no child, then parents child points to incoming child
    // Else we find a place in the sibling list    
    //if (parent->child == 0)
    if (!(parent->child) )
    {
        //inChild->parent = parent; // Assign Parent as child's parent
        parent->child = inChild;            // Assign inChild as Parent's child
        parent->child->parent = parent;
    } else {
        
        PROC *temp = parent->child; //This will be our temp child iterator
        //! We iterate until we find an empty sibling spot
        while(temp->sibling != 0)
        {
            temp = temp->sibling;
        }
        //inChild->parent = parent;
        temp->sibling = inChild;
        temp->sibling->parent = parent;
    }
}


int timerEnqueue(PROC **queue, PROC *p) //! different thean pipe.tgz
{
    PROC *q = *queue;
    PROC *decrementFromhere;
    PROC *previous;

    //Here is when the proc we are trying to put in time is < current head
    // or the incoming proc priority is less then head priority
    // then we make the incoming proc head and decrement any existing procs by that amount
    if (q == 0 || p->priority < q->priority)
    {
        *queue = p;
        p->next = q;
        decrementFromhere = q;

        q->priority -= p->priority;

        //* We have to decrement the rest of the queue, Not True
        //* We only have to decrement the next over!
/*         while(q)
        {
            q->priority -= p->priority;
            q = q->next;
        } */
        return;
    }
    previous = q;
    while (q->next && p->priority >= q->next->priority)
    {
        //TODO: Need to take care of thise case!!
        //! each move we make we decrement our value by the current value
        p->priority -= q->next->priority;
        previous = q;
        q = q->next;        
    }
    //Then here we have to decrement the by the time left over on p?
    //previous->next = p;
    int decrementValue = p->priority;
    PROC *temp = q->next;
    q->next = p;
    p->next = temp;
    decrementFromhere = temp;
    //! Should only need to decrement the next over
    decrementFromhere->priority -= decrementValue;

    //! This following loop will decrement each one, not what we need!
/*     while (decrementFromhere)
    {
        decrementFromhere->priority -= decrementValue;
        //TODO: There may be a case where a proc here may go negative?
    }     */
    //p->next = q->next;
    //q->next = p;
}

int inOrderTimerRequeue(PROC **queue, PROC *p)
{
    PROC *q = *queue;
    if(q == 0)
    {
        *queue = p;
        p->next=q;
        return;
    }
    while(q->next)
    {
        q = q->next;
    }
    q->next = p;
}

int printTQE(PROC *p)
{
    printf("TimeQueue =");
    while (p)
    {
        printf("[Proc[%d]|RTime[%d]->", p->pid, p->priority);
        p = p->next;
    }
    printf("NULL\n");
}

int enqueue(PROC **queue, PROC *p) //! different thean pipe.tgz
{
    PROC *q = *queue;
    if (q == 0 || p->priority > q->priority)
    {
        *queue = p;
        p->next = q;
        return;
    }
    while (q->next && p->priority <= q->next->priority)
    {
        q = q->next;
    }
    p->next = q->next;
    q->next = p;
}

PROC *dequeue(PROC **queue)
{
    PROC *p = *queue;
    if (p)
        *queue = p->next;
    return p;
}

int printQ(PROC *p)
{
    kprintf("readyQueue = ");
    while (p)
    {
        kprintf("[%d%d]->", p->pid, p->priority);
        p = p->next;
    }
    kprintf("NULL\n");
}

int printQueue(PROC *p)
{
    while (p)
    {
        kprintf("[%d%d]->", p->pid, p->priority);
        p = p->next;
    }
    kprintf("NULL\n");
}

int printList(char *name, PROC *p)
{
    printf("%s=", name);
    while (p)
    {
        printf("[%d%d]->", p->pid, p->priority);
        p = p->next;
    }
    printf("NULL\n");
}

int printsleepList(PROC *p)
{
    printf("sleepList =");
    while (p)
    {
        printf("[%devent=%x]->", p->pid, p->event);
        p = p->next;
    }
    printf("NULL\n");
}
