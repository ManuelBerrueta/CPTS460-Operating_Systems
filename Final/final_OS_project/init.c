#include "ucode.c"
#define printk printf

int console, serial0, serial1;

//! Reference from book: 8.15.1 The INIT Program
int parent() // P1's code
{
    int pid, status;
    
    while (1)
    {
        printf("BERR INIT : wait for ZOMBIE child\n");

        pid = wait(&status);

        if (pid == console) // if console login process died
        { 
            printf("BERR INIT: forks a new console login\n");
            console = fork(); // fork another one
            if (console)
                continue;
            else
                exec("login /dev/tty0"); // new console login process
        }

        // Serial0 Terminal
        if (pid == serial0)
        {
            printf("BERR INIT %d: fork a login proc on console\n", pid);
            serial0 = fork();

            if (serial0)
            {
                continue; 
            }
            else
                exec("login /dev/ttyS0"); // new serial console login process   
        }

        // Serial1 Terminal
        if (pid == serial1)
        {
            printf("BERR INIT %d: fork a login proc on console\n", pid);
            serial1 = fork();
            if (serial1) 
            {
                continue; 
            }
            else
                exec("login /dev/ttyS1"); // new serial console login process   
        }

        printf("BERR INIT: I just buried an orphan child proc %d\n", pid);
    }
}


int main(int argc, char *argv[])
{
    int in, out;  // File descriptors for terminal I/O
    int pid, status;

    in = open("/dev/tty0", O_RDONLY);  // File Descriptor 0
    out = open("/dev/tty0", O_WRONLY); // File Descriptor 1 for console display

    pid = getpid();

    printf("BERR INIT %d: fork a login proc on console\n", pid);

    console = fork();

    if (console) // Parent
    {
        serial0 = fork();
        if (serial0)
        {
            serial1 = fork();
            if (serial1)
            {
                parent();
            }
            else
            {
                exec("login /dev/ttyS1");
            }
        }
        else
        {
            exec("login /dev/ttyS0");
        }
    }
    else // otherwise, it's the child PROC; login on tty0
    {
        exec("login /dev/tty0");
    }
}
