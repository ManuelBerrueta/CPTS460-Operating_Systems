#include "type.h"
#include "message.c"

int sender() // send task code
{
    struct uart *up = &uart[0];
    char line[128];
    while (1)
    {
        ugets(up, line);
        printf("task%d got a line=%s\n", running->pid, line);
        send(line, 4);
        printf("task%d send %s to pid=4\n", running->pid, line);
    }
}

int receiver() // receiver task code
{
    char line[128];
    int pid;
    while (1)
    {
        printf("task%d try to receive msg\n", running->pid);
        pid = recv(line);
        printf("task%d received: [%s] from task%d\n",
               running->pid, line, pid);
    }
}

int main()
{
    msg_init();
    kprintf("P0 kfork tasks\n");
    kfork((int)sender, 1);   // sender process
    kfork((int)receiver, 1); // receiver process
    while (1)
    {
        if (readyQueue)
            tswitch();
    }
}