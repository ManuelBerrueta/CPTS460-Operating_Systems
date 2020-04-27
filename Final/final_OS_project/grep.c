#include "ucode.c"
#define printk printf

int grep(char *pattern, char *buff)
{
    int i = 0, j = 0, tempStrSize = 0, grepSize = 0;
    tempStrSize = strlen(buff);
    grepSize = strlen(pattern);

    for(i=0; i < tempStrSize; i++)
    {
        if(buff[i] == pattern[j])
        {
            j++;
            if (j == grepSize)
            {
                printf("%s\n", buff);
                j=0;
                break;
            }
        } else {
            j = 0;
        }
    }
}


int main(int argc, char *argv[])
{
    char tempStr[1024] = { 0 }, grepStr[1024] = { 0 }, searchStr[1024] = { 0 };
    int i = 0, j = 0, k = 0, tempStrSize = 0, grepSize = 0, fd = 0, n = 0;
    int filePosTracker = 0;

    printf(">>>>>>>>{ BERRNIX grep }<<<<<<<<\n");
    
    if (argc == 2) //grep froms stdin, pattern is argv[1]
    {
        strcpy(grepStr, argv[1]);
        grepSize = strlen(grepStr);
        while (1)
        {
            j = 0;
            gets(tempStr);

            if (strcmp(argv[1], tempStr) == 0) //Simple case, is exactly the same pattern
            {
                printf("%s\n", tempStr);
            } else {
                grep(grepStr, tempStr);
            }
        }
    }
    else if (argc == 3)
    {
        strcpy(grepStr, argv[1]);
        fd = open(argv[2], O_RDONLY);
        if (fd < 0)
        {
            printf("-=0={ERROR: File '%s' FAILED to OPEN\n", argv[2]);
            return 1;
        }
        
        // read a block worth of data
        while (n = read(fd, tempStr, 1024))
        {
            // We must locate every line '\n' and return '\r'
            // We must keep track of where we are on the file for every line
            // So we can continue from there
            
            for(i=0; i < n; i++)
            {
                if ((tempStr[i] == '\n') || (tempStr[i] == '\r'))
                {
                    strncpy(searchStr, tempStr, i+1);
                    searchStr[i+1] = 0;
                    filePosTracker++;
                    break;
                }
                // Use filePosTracker to keep track of what byte/position we are on
                filePosTracker++;
            }


            grep(grepStr, searchStr);

            // Use lseek to put us back where we broke off the current stream
            lseek(fd, filePosTracker, 0);
        }
        //TODO: open a file and look for pattern

    } else {
        printf("ERROR: argc=%d, grep requires at least 1 or 2 args only!\n ", argc);
    }

    return 0;
}