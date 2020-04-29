#include "ucode.c"
#define printk printf



int myPrintMore(inFile, numOfLines)
{
    int i = 0, j = 0;
    char eachChar[2];
    int endOfFile = 1;

    printf("Inside myPrintMore\n");

    while ( i < numOfLines )
    {
        j=0;
        while (j <= 80)
        {
            // Read one char at a time and print
            endOfFile = read(inFile, eachChar, 1);
            
            if(endOfFile < 1)
            {
                printc("\n");
                exit(100);
            }
            else if (eachChar[0] == '\n')
            {
                printc("\n");
                //return;
            } else {
                //printf("%c", eachChar[0]); This did not work?!
                printc(eachChar[0]);
            }
            j++;
        }
        i++;
    }
    return;
}


int main(int argc, char *argv[])
{
    char mybuff[1024], fileName[1024], inputChar; //a null char at end of mybuff[]
    int n, fd;

    printf(">>>>>>>>{ BERRNIX more }<<<<<<<<\n");

    //if ((argc == 1) || (fileName == 0) || (strcmp(fileName, "") == 0) || (strcmp(fileName, " ") == 0))
    if (argc == 1)
    {
        fd = dup(0);
        close(0);

        gettty(tty);
        open(tty, O_RDONLY);

        while (1)
        {
            gets(mybuff);
            printf("%s\n", mybuff);
            if (mybuff == 0x04)
            {
                printf("Exit more\n");
                exit(1);
            }
        }
    } else {
        strcpy(fileName, argv[1]);
        fd = open(fileName, O_RDONLY);
        if(fd < 0)
        {
            printf("-=0={ERROR: File '%s' FAILED to OPEN\n", fileName);
        }
    
        printf("-=0=[Start more file: '%s' @ fd=%d\n", fileName, fd);
        puts("===================================================");

        myPrintMore(fd, 25);

        while (1)
        {
            // Get input char
            //gets(inputChar);
            inputChar = getc();

            if (inputChar == 'q')
            {
                close(fd);
                return 0;
            }
            else if (inputChar == ' ') //if the input is a space we do 25 lines
            {
                myPrintMore(fd, 25);
            }
            else if (inputChar == '\r')
            {
                myPrintMore(fd, 1);
            }
        }

        puts("===================================================");
        printf("-=0=[END more file: '%s' @ fd=%d\n", fileName, fd);
        close(fd);
    }
    return 0;
}
