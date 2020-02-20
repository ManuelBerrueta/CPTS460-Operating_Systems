#include "type.h"

/* #define NMBUF 10
struct semaphore nmbuf, mlock;
MBUF mbuf[NMBUF], *mbufList;         // mbufs buffers and mbufList */

extern struct semaphore nmbuf, mlock;
extern MBUF mbuf[NMBUF], *mbufList;         // mbufs buffers and mbufList
 

int menqueue(MBUF **queue, MBUF *p) //{// enter p into queue by priority}
{
    MBUF *q = *queue;
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

MBUF *mdequeue(MBUF **queue) //return first queue element}
{
    MBUF *p = *queue;
    if (p)
    {
        *queue = p->next;
    }
    return p;
} 

int msg_init()
{
    int i;
    MBUF *mp;
    printf("mesg_init()\n");
    mbufList = 0;
    for (i = 0; i < NMBUF; i++)        // initialize mbufList
        menqueue(&mbufList, &mbuf[i]); // all priority=0, so use menqueue()
    nmbuf.value = NMBUF;
    nmbuf.queue = 0; // counting semaphore
    mlock.value = 1;
    mlock.queue = 0; // lock semaphore
}

MBUF *get_mbuf() // allocate a mbuf
{
    P(&nmbuf);
    P(&mlock);
    MBUF *mp = mdequeue(&mbufList);
    V(&mlock);
    return mp;
}

int put_mbuf(MBUF *mp) // release a mbuf
{
    P(&mlock);
    menqueue(&mbufList, mp);
    V(&mlock);
    V(&nmbuf);
}

int send(char *msg, int pid) // send msg to partet pid
{
    /* if (checkPid() < 0) // validate receiving pid
        return -1; */

    if(pid < 0)
    {
        return -1;
    }
    
    PROC *p = &proc[pid];
    MBUF *mp = get_mbuf();

    printf("==\t In send(): pid=%d mp= int:%d  hex:%x\n", pid, mp, mp);

    mp->pid = running->pid;
    //mp->pid = (MBUF *)running->pid;
    mp->priority = 1;

    strcpy(mp->contents, msg);
    printf("===mp->contents: %s  |  msg:%s\n", mp->contents, msg);

    
    P(&p->mQlock);      //!Possibly broken here

    menqueue(&p->mQueue, mp);

    printf("\t\t====P(&p->mQlock)\n");

    
    V(&p->mQlock);
    V(&p->nmsg);

    printf("\t\t====SENT LINE TO RECV\n");
    return 0;
}

int recv(char *msg) // recv msg from own msgqueue
{
    printf("\n\n**Inside recv\n");
    P(&running->nmsg);
    
    printf("\n\n**Inside recv2\n");
    P(&running->mQlock);
    
    //Debug

    MBUF *mp = mdequeue(&running->mQueue);
    
    printf("**mp->contents: %s\n", mp->contents);
    
    V(&running->mQlock);
    strcpy(msg, mp->contents);
    int sender = mp->pid;
    
    put_mbuf(mp);

    return sender;
}