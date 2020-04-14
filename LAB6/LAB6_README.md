               CS460 LAB Assignment 6

	         DUE & DEMO 4-9-2020
              Turn in your zip file to TA

READ: Chapter 7.7.5: fork-exec in Unix/Linux
      Chapter 7.7.6: implementation of fork
      Chapter 7.7.7:Implementation of exec

1. BASE CODE: your LAB5pre step 3.

2. Download files from samples/LAB6/USER into a USER directory
   mku u1 ===> generates /bin/u1 on sdimage

/*********** u1.c contents ******************/
include "ucode.c"

int main(int argc, char *argv[])
{
  int i, pid, ppid, r;
  char line[64]; char uc;

  for (i=0; i < argc; i++){
    printf("argv[%d] = %s\n", i, argv[i]);
  }

  while(1){
    pid  = getpid();
    ppid = getppid();
    printf("This is process %d in Umode at %x parent=%d\n",
           pid, getPA(), ppid);

    umenu();
    printf("input a command : ");
    ugets(line);
    uprintf("\n");

    if (strcmp(line, "ps")==0)
      ups();
    if (strcmp(line, "switch")==0)
       uswitch();
    if (strcmp(line, "exit")==0)
       uexit();

    if (strcmp(line, "wait")==0)
       uwait();
    if (strcmp(line, "fork")==0)
       ufork();
    if (strcmp(line, "exec")==0)
       uexec();
  }
}


//-------------- ucode.c has these syscalls ----------------------
int ufork()
{
  int pid;

  pid = syscall(9,0,0,0); // syscall 9 is for fork()
  if (pid>0){
    uprintf("parent %d forked a child %d\n", getpid(), pid);
  }
  if (pid==0){
    uprintf("child %d return from fork(), pid=%d\n", getpid(), pid);
  }
  if (pid < 0)
    uprintf("%d fork failed\n", getpid());
}

int uexec()
{
  int r;
  char line[64];

  uprintf("enter a command string : ");
  ugets(line);
  uprintf("line=%s\n", line);
  r = syscall(10,line,0,0); // syscall 11 is for exec(line)
  if (r<0)
     uprintf("%d exec failed\n", getpid());
}

//---------------------------------------------------------------
So, you must add syscall numbers 9 and 10 to your svc.c file


                    REQUIREMENTS

PART 1: 50 points

1. Implement int fork() in kernel.
2. Implement int exec(char *cmdLine) in kernel.
             where cmdLine = input line from user for exec, e.g.
                   exec
                   enter a cmdLine:  u2 one two three  <== cmdline to exec().


PART2: 50 points

In Chapter 7, the sample code of fork/exec (which is YOUR PART 1) assume each 
Umode image size is 1 MB at
                              7MB + pid*1MB.
Modify it to: Each Umode image size is 2MB at the physical addresses
              First  1MB at:  7MB + pid*1MB 
              Second 1MB at: 16MB + pid*1MB


Sample solution: samples/LAB6/t.bin

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


                     LAB6 HELP

0. ts.s file for kernel: focus on svc_entry,goUmode:

   svc_entry:
       stmfd sp!, {r0-r12, lr}     // save Umode registers in kstack
       save Umode cpsr in PROC.spsr;
                  usp  in PROC.usp
       call svc_hanler to execute a kernel function;
       put return value r into saved R0 in kstack

   goUmode:
       restore spsr from PROC.spsr;
               usp  from PROC.usp;
       ldmfd sp!, {r0-r12, pc}^   // back to Umode with r in R0
		     
1. Each process, except P0, has a 2MB user mode image at
   1st  1MB at   7MB + pid*1MB
   2nd  1MB at  16MB + pid*1MB

SO each process needs TWO user mode pgdir entries [2048] -> 1st 1MB
                                                  [2049] -> 2nd 1MB

These TWO 1MB areas are not contiguous physically but are contiguous as VA.

Refer to the following diagram

    PROC

    pgdir ----> 4MB+pid*16KB
                ===================
                 0 -----> PA 0   MB         
                 1 -----> PA 1   MB
                 ....
		257 ----> PA 257 MB
		===================  
   2MB VA
 -----------
 |  1MB    |    2048 ---->1st 1MB of Uimage, e.g. at 8MB
 |  2MB    |    2049 ->
 -----------          |   they are NOT adjacent !!!
                      |
	              |-->2nd 1MB of Uimage, e.g. at 16MB

2. kfork("u1") creates P1 with 2MB Umode image: one at 8MB, another at 16MB
   Load u1 to its first 1MB is good enough since u1 size is only about 5KB.

   HOWEVER, P1's usp must point at the HIGH end of its VA, i.e.
            p->usp = VA(2MB) = 0x80200000;

   You may get a data exception when P1 runs u1 in Umode if its R0 points to a
   invalid VA. Try to set R0 at p->kstack[SSIZE-14]=VA(2MB - 4);
			 

   To test YOUR code used the 2MB VA or not, in u1,
      define a LOCAL varialbe
               int p;
	       printf("&p = %x\n", &p);
      it should be 0x801FFFxx
			 			 i
3. fork(): Caller Umode image is 2MB (two separate piceces of 1MB PA).
           MUST copy both 1MB areas to child's Umode image.

  
4. exec(commandLine):
   load file to 1st MB area.
   Make ustack conatin  -----------------------------------------------
                                               |commandLine           |
                        -----------------------|-----------------------	
					     -128                    2MB VA
                                               
   Umode sp:      p->usp = VA(2MB-128)                // ustack pointer VA 

   Umode R0:      p->kstack[SSIZE-14] = VA(2MB-128)   // passed to main0(s)

   Umode lr:      p->kstack[SSIZE- 1] = VA(0)         // entry of us.s at VA 0


5. Use the same technique to pass a line "u1 start" to P1 when it starts to
   execute u1 in Umode.
