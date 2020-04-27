#include "ucode.c"
#define printk printf
#define BLKSIZE 1024

int cp(char source[], char dest[])
{
    int fd = open(source, O_RDONLY);
    int gd = open(dest, O_WRONLY);
    int n = 0;
    char cpybuff[BLKSIZE];

    if(fd < 0)
    {
        printf("-=0={ERROR: File: %s FAILED to open\n", source);
    }
    if(gd < 0)
    {
        printf("-=0={ERROR: File: %s FAILED to open\n", source);
        printf("-=0={CREAT File: %s \n", source);
        //creat(dest);

        gd = open(dest, O_WRONLY | O_CREAT);
        if(gd < 0)
        {
            printf("-=0={ERROR: CREAT File: %s \n", source);
        }
    }

    int debug = 0;
    while(n = read(fd, cpybuff, 1024))
    {
        if (debug == 11) 
            debug *= -1;

        write(gd, cpybuff, n);
        memset(cpybuff, 0, BLKSIZE);

        debug++;
    }
    close(fd);
    close(fd);
    return 0;
}


int main(int argc, char const *argv[])
{
    printf(">>>>>>>> BERRNIX cp %s to %s <<<<<<<<\n", argv[1], argv[2]);
    cp(argv[1], argv[2]);
    printf(">>>>>>>> BERRNIX cp SUCCESS?! <<<<<<<<\n", argv[1], argv[2]);
    
    return 0;
}

