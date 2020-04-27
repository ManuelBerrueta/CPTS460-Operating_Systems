#include "ucode.c"
#define printk printf


int main(int argc, char *argv[])
{
    char tempStr[1024] = { 0 }, grepStr[1024] = { 0 };
    int i = 0, j = 0, k = 0, tempStrSize = 0, grepSize = 0;

    printf(">>>>>>>>{ BERRNIX grep }<<<<<<<<\n");
    
    if (argc == 2) //grep froms stdin, pattern is argv[1]
    {
        strcpy(grepStr, argv[1]);
        grepSize = strlen(grepStr);
        while (1)
        {
            j = 0;
            gets(tempStr);
            tempStrSize = strlen(tempStr);

            if (strcmp(argv[1], tempStr) == 0) //Simple case, is exactly the same pattern
            {
                printf("%s\n", tempStr);
            } else {
                
                for(i=0; i < tempStrSize; i++)
                {
                    if(tempStr[i] == grepStr[j])
                    {
                        j++;
                        if (j == grepSize)
                        {
                            printf("%s\n", tempStr);
                            j=0;
                            break;
                        }
                    }
                }
            }
        }
    }
    else if (argc == 3)
    {
        
        //TODO: open a file and look for pattern

    } else {
        printf("ERROR: argc=%d, grep requires at least 1 or 2 args only!\n ", argc);
    }

    return 0;
}