typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#include "string.c"
#include "uio.c"
#include "ucode.c"

int main()
{
    int pid, ppid;
    char line[64];
    u32 mode, *up;

    mode = getcpsr();
    mode = mode & 0x1F;
    printf("CPU mode=%x\n", mode); // verify we are in USER mode

    pid = getpid();
    ppid = getppid();

    while (1)
    {
        printf("Este es el processo %d en Umode padre=%d\n", pid, ppid);
        umenu();
        printf("Entre un commando : ");
        ugets(line);
        printf("\n");

        if (strcmp(line, "damepid") == 0)
            ugetpid();
        if (strcmp(line, "dameppid") == 0)
            ugetppid();
        if (strcmp(line, "ps") == 0)
            ups();
        if (strcmp(line, "chname") == 0)
            uchname();
        if (strcmp(line, "cambiar") == 0)
            uswitch();
        if (strcmp(line, "damepa")==0)
            getPA();
    }
}
