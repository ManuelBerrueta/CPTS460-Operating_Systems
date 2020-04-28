#include "ucode.c"
#define printk printf


int main(int argc, char *argv[])
{
    char mybuff[1024], fileName[1024], dummy = 0; //a null char at end of mybuff[]
    int n;

    printf(">>>>>>>>{ BERRNIX CAT }<<<<<<<<\n");

    if (argc == 1)
    {
        while (1)
        {
            gets(mybuff);
            printf("%s\n", mybuff);
            if (mybuff == 0x04)
            {
                printf("Exit CAT\n");
                exit(1);
            }
        }
    } else {
        strcpy(fileName, argv[1]);
        int fd = open(fileName, O_RDONLY);
        if(fd < 0)
        {
            printf("-=0={ERROR: File '%s' FAILED to OPEN\n", fileName);

        }
        else
        {
            printf("-=0=[Start cat file: '%s' @ fd=%d\n", fileName, fd);
            puts("===================================================");
        }


        int debug = 0;
        while(n = read(fd, mybuff, 1024))
        {
            if (debug == 11) 
                debug *= -1;

            mybuff[n] = 0;
            printf("%s", mybuff);
            //TODO: HANDLE '\n'
            debug++;
        }
        puts("===================================================");
        printf("-=0=[END cat file: '%s' @ fd=%d\n", fileName, fd);
        close(fd);
    }
    return 0;
}