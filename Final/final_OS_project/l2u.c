#include "ucode.c"
#define printk printf


int l2u(char *buf, int fd, int outFile)
{
    char low2upBuff[1024] = {0};
    int i = 0;
    int firstLowCaseChar = 97, lastLowCaseChar = 122;

    int size = strlen(buf);

    for (i=0; i < size; i++)
    {
        if (buf[i] >= firstLowCaseChar && buf[i] <= lastLowCaseChar)
        {
            low2upBuff[i] = buf[i] - 32;
        }
        else if (buf[i] == '\r')
        {
            low2upBuff[i] = '\n';
        } else {
            low2upBuff[i] = buf[i];
        }
    }

    if (fd && outFile)
    {
        //printf("%s\r", low2upBuff);
        write(outFile, low2upBuff, size);
        
    } else {
        printf("%s\n", low2upBuff);
    }

    return ;
}


int main(int argc, char *argv[])
{
    char mybuff[1024], fileName[1024], dummy = 0; //a null char at end of mybuff[]
    int n, fd = 0, outFile = 0;

    printf(">>>>>>>>{ BERRNIX l2u }<<<<<<<<\n");

    //if ((argc == 1) || (fileName == 0) || (strcmp(fileName, "") == 0) || (strcmp(fileName, " ") == 0))
    if (argc == 1)
    {
        while (1)
        {
            gets(mybuff);

            l2u(mybuff, fd, outFile);
            //printf("%s\n", mybuff);
            if (mybuff == 0x04)
            {
                printf("Exit CAT\n");
                exit(1);
            }
        }
    }
    else if (argc == 3)
    {
        strcpy(fileName, argv[1]);
        fd = open(argv[1], O_RDONLY);
        outFile = open(argv[2], O_WRONLY | O_CREAT);
        if (fd < 0)
        {
            //printf("-=0={ERROR: File '%s' FAILED to OPEN\n", fileName);
        }
        else
        {
            //printf("-=0=[Start cat file: '%s' @ fd=%d\n", fileName, fd);
            puts("===================================================");
        }

        int debug = 0;
        while (n = read(fd, mybuff, 1024))
        {
            if (debug == 11)
                debug *= -1;

            mybuff[n] = 0;
            //printf("%s", mybuff);
            l2u(mybuff, fd, outFile);
            //TODO: HANDLE '\n'
            debug++;
            if(n < 1024)
            {
                break;
            }

        }
        puts("===================================================");
        printf("-=0=[END cat file: '%s' @ fd=%d\n", fileName, fd);
        close(fd);
    } else {
        printf("Error argc != 3!\n");
    }
    return 0;
}
