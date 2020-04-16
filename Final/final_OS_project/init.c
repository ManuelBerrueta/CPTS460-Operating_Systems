#include "ucode.c"
#define printk printf



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

int console, console1, console2;

int main(int argc, char *argv[])
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
}
