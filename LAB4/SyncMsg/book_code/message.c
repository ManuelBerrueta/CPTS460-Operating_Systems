#include "type.h"

#define NMBUF 10
struct semaphore nmbuf, mlock;
MBUF mbuf[NMBUF], *mbufList;         // mbufs buffers and mbufList
int menqueue(MBUF **queue, MBUF *p); //{// enter p into queue by priority}
MBUF *mdequeue(MBUF **queue);        //{// return first queue element}

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
    if (checkPid() < 0) // validate receiving pid
        return -1;
    PROC *p = &proc[pid];
    MBUF *mp = get_mbuf();
    mp->pid = running->pid;
    mp->priority = 1;
    strcpy(mp->contents, msg);
    P(&p->mQlock);
    menqueue(&p->mQueue, mp);
    V(&p->mQlock);
    V(&p->nmseg);
    return 0;
}

int recv(char *msg) // recv msg from own msgqueue
{
    P(&running->nmsg);
    P(&running->mQlock);
    MBUF *mp = mdequeue(&running->mQueue);
    V(&running->mQlock);
    strcpy(msg, mp->contents);
    int sender = mp->pid;
    put_mbuf(mp);
    return sender;
}