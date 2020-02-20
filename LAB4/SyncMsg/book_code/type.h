#ifndef TYPE_H
#define TYPE_H


struct semaphore
{
    int value;
    struct proc *queue;
};

typedef struct mbuf
{
    struct mbuf *next;
    int priority;
    int pid;
    char contents[128];
}MBUF;

typedef struct proc
{
    // same as before, but added
    MBUF *mQueue;
    struct semaphore mQlock;
    struct semaphore nmsg;
    int kstack[SSIZE];
}PROC;


#endif