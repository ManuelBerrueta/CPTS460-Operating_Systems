int block(struct semaphore *s)
{
    running->status = BLOCK;
    enqueue(&s->queue, running);
    //menqueue(&s->queue, running);
    tswitch();
}

int signal(struct semaphore *s)
{
    PROC *p = dequeue(&s->queue);
    //MBUF *p = mdequeue(&s->queue);

    p->priority = READY;
    enqueue(&readyQueue, p);
    //menqueue(&mreadyQueue, p);
}

int P(struct semaphore *s)
{
    int SR = int_off();
    
    s->value--;
    if (s->value < 0)
    {
        block(s);
    }
    
    int_on(SR);
}

int V(struct semaphore *s)
{
    int SR = int_off();
    s->value++;
    if (s->value <= 0)
    {
        signal(s);
    }
    int_on(SR);
}