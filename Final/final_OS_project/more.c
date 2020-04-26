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
            
            if(endOfFile < 0)
            {
                return;
            }
            else if (eachChar[0] == '\n')
            {
                printf("\n");
                return;
            } else {
                //printf("%c", eachChar[0]); This did not work?!
                princ(eachChar[0]);
            }
            j++;
        }
        i++;
    }
    return;
}


int main(int argc, char *argv[])
{
    char mybuff[1024], fileName[1024], inputChar[2] = {0}; //a null char at end of mybuff[]
    int n;

    printf(">>>>>>>>{ BERRNIX more }<<<<<<<<\n");

    //if ((argc == 1) || (fileName == 0) || (strcmp(fileName, "") == 0) || (strcmp(fileName, " ") == 0))
    if (argc == 1)
    {
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
        int fd = open(fileName, O_RDONLY);
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
            gets(inputChar);

            if (inputChar[0] == 'q')
            {
                close(fd);
                return 0;
            }
            else if (inputChar[0] == ' ') //if the input is a space we do 25 lines
            {
                myPrintMore(fd, 25);
            }
            else if (inputChar[0] == '\r')
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
