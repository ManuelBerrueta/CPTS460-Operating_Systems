// wait.c file

extern PROC *running;
extern PROC *sleepList;

int kexit()  // SIMPLE kexit() for process to terminate
{
  printf("proc %d exit\n", running->pid);
  running->status = ZOMBIE;
  tswitch();
}

int ksleep(int event)
{
  // implement this
}

int kwakeup(int event)
{
  // implement this
}
