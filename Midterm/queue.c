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
        
        //NEW Line debugging 
        //parent->child->ppid = parent->pid;
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


//! Print Child List
int printChildList(PROC *p)
{
    PROC *temp = p;
    char statusStr[10];
    printf("Proc[%d] Child List:\n", p->pid);
    if (temp->child)
    {
        if(temp->child->status == 1) //1 is ready, 4 is zombie
        {
            //TODO: Need to change status from int to a string of ZOMBIE or READY
            strcpy(statusStr, "READY");
        } else if (temp->child->status == 4)
        {
            strcpy(statusStr, "ZOMBIE");
        }

        printf("[%d, %s]->", temp->child->pid, statusStr);
        //temp = temp->sibling;
        temp = temp->child->sibling;
        if(temp)
        {
            while (temp)
            {
                printf("[%d, READY]->", temp->pid);
                temp = temp->sibling;
            }
        }
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