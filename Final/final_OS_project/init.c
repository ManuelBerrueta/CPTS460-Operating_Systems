#include "ucode.c"
#define printk printf

int console, serial0, serial1;

//! ORIGINAL CODE
/* int main(int argc, char *argv[])
{
    int in, out;
    int pid, status;

    in = open("/dev/tty0", O_RDONLY);
    out = open("/dev/tty0", O_WRONLY);

    pid = getpid();

    printf("BERR INIT %d: fork a login proc on console\n", pid);

    console = fork();
    if (console)
    { // parent
        while (1)
        {
            pid = wait(&status);
            if (pid == console)
            { // login child died
                printf("BERR INIT %d: fork another login on console\n", pid);
                console = fork();
                if (console)
                {
                    continue;
                }
                else
                { // child
                    exec("login /dev/tty0");
                }
            }
            printf("KCW INIT burried an orphan proc %d\n", pid);
        }
    }
}
 */

//! My old code
/*
    console = fork();
    if (console)
    { // parent

        while (1)
        {
            pid = wait(&status);
            if (pid == console)
            { // login child died
                printf("BERR INIT %d: fork another login on console\n", pid);
                console = fork();
                if (console)
                {
                    //continue;
                    console1 = fork();
                    if (console1)
                    {
                        console2 = fork();
                        if (console2)
                        {
                            continue;
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
                else
                { // child
                    exec("login /dev/tty0");
                }
            }
            printf("KCW INIT burried an orphan proc %d\n", pid);
        }
    }
    */


//! Reference from book: 8.15.1 The INIT Program
int parent() // P1's code
{
    int pid, status;
    while (1)
    {
        printf("INIT : wait for ZOMBIE child\n");
        pid = wait(&status);
        if (pid == console)
        { // if console login process died
            printf("INIT: forks a new console login\n");
            console = fork(); // fork another one
            if (console)
                continue;
            else
                exec("login /dev/tty0"); // new console login process
        }


        // serial0 Terminal
        if (pid == serial0)
        {
            printf("BERR INIT %d: fork a login proc on console\n", pid);
            serial0 = fork();

            if (serial0)
            {
                //continue;
                        // serial1 Terminal
                if (pid == serial1)
                {
                    printf("BERR INIT %d: fork a login proc on console\n", pid);
                    serial1 = fork();

                    if (serial1)
                        continue;
                    else
                        exec("login /dev/ttyS1"); // new serial console login process   
                }
            }
            else
                exec("login /dev/ttyS0"); // new serial console login process   
        }




        printf("INIT: I just buried an orphan child proc %d\n", pid);
    }
}



int main(int argc, char *argv[])
{
    int in, out;  // File descriptors for terminal I/O
    int pid, status;

    in = open("/dev/tty0", O_RDONLY);
    out = open("/dev/tty0", O_WRONLY);

    pid = getpid();

    printf("BERR INIT %d: fork a login proc on console\n", pid);

    console = fork();
    if (console)
    { // parent

        while (1)
        {
            pid = wait(&status);
            if (pid == console)
            { // login child died
                printf("BERR INIT %d: fork another login on console\n", pid);
                console = fork();
                if (console)
                {
                    //continue;
                    serial0 = fork();
                    if (serial0)
                    {
                        serial1 = fork();
                        if (serial1)
                        {
                            continue;
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
                else
                { // child
                    exec("login /dev/tty0");
                }
            }
            printf("KCW INIT burried an orphan proc %d\n", pid);
        }
    }
}
