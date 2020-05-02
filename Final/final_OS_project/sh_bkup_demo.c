#include "ucode.c"
#define printk printf
#define DEBUG 1

int stdin, stdout, err;

int executeCommand(char buff[]);
int pipeCheck(char buff[]);

int main(int argc, char *argv[])
{
    char buff[256] = {0};
    char parseBuff[256] = {0};
    char *myargv[246] = {0};
    char command[16] = {0}; //? Command string
    char cwd[256];
    int myargc = 0;
    int i = 0;
    int argcounter = 0;
    char tempStr[128] = {0};


    //! Get user input
    while (1)
    {
        if (getcwd(cwd) == 0)
        {
            printf("getcwd Failed!\n\n");
        }

        //printf("[ %04d/%02d/%02d ] BERR Shell [ %s ]\n|-$ ", tm.tm_year + 1900, tm.tm_mon, tm.tm_mday, cwd);
        printf("BERR Shell #%d [ %s ]-$ ", getpid(), cwd);
        gets(buff);
        
        // Check and get rid of '\n'
        if (buff[strlen(buff) - 1] == '\n')
            buff[strlen(buff) - 1] = 0; // *Get rid of '\n'

        // Take care of the case where buff is empty so shell does not crash
        if( (strlen(buff) == 0) || (strcmp(buff, "") == 0) || (buff[0] == " "))
        {
            buff[0] = " ";
        }
        strcpy(parseBuff, buff);
        printf("COMMAND = [%s]\n", parseBuff);

        while (buff[i] != '\0')
        {
            if (buff[i] == ' ')
            {
                argcounter++;
            }
            i++;
        }

        //! Tokenize command and parameters
        i = 0;
        strcpy(command, strtok(parseBuff, " "));
        myargv[i] = command;
        i++;
        if (argcounter > 0)
        {
            while (i <= argcounter)
            {
                myargv[i] = strtok(NULL, " ");
                i++;
            }
            i = 0;
        }

        //! Check if command == cd
        if (strcmp(command, "cd") == 0)
        {
            if (argcounter != 0)
            {
                chdir(myargv[1]);
            }
            else
            {
                chdir('~');
            }
            argcounter = 0;
        }
        //! Check if command == exit
        else if ((strcmp(command, "exit") == 0) || (strcmp(command, "logout") == 0))
        {
            exit(1);
        }
        else if (strcmp(command, "pwd") == 0)
        {
            printf("%s\n", cwd);
        }
        else
        {
            //! If is not cd or exit check for pipes
            //* Check for pipes & run command
            int local_pid  = fork();
            if(local_pid)
            {
                int local_status = 0;
                local_pid = wait(&local_status);
                printf("local_pid %d terminated with exit(%d)\n", local_pid, local_status);
                memset(buff, '\0', 256);
                //buff[0] = " ";
            } else {
                pipeCheck(buff);
                exit(100);
            }
        }
    }
    return 0;
}

int executeCommand(char buff[])
{
    int i = 0;
    int j = 0;
    int k = 0;
    int argcounter = 0;
    char tempArg[64] = "\0";
    char *pathNames[36] = {0};
    int stdinFlag = 0;
    int stdoutFlag = 0;
    int stdoutAppen = 0;
    char redirectName[64] = {0};
    const char path[512] = {0};
    char *myargv[246] = {0}; //? MAY NEED TO REPLACE this with one passed in?
    char command[16];        //? Command string

    strcpy(tempArg, buff); //TODO: if argcounter ==1 just run tempArg


    //! (Count number of spaces, check for redirection and pipes
    while (buff[i] != '\0')
    {
        if (buff[i] == ' ')
        {
            argcounter++;
        }
        else if (buff[i] == '>' && buff[i + 1] == '>') //! >> redirection check
        {
            if (i != 0)
            {
                stdoutAppen = i;
                //!Clear buff of the redirect
                buff[i] = 0; //* Gets rid of the first '>'
                buff[i + 1] = 0; //* Gets rid of the second '>'
                buff[i + 2] = 0; //* Gets rid of the space after the last

                j = i + 3;
                int k = 0;
                
                //* Do the string copy here & clean buff string
                while (buff[j] != '\0')
                {
                    redirectName[k++] = buff[j];
                    buff[j++] = 0; //!delete the rest of none command chars
                }
                redirectName[k] = 0;
            }
        }
        else if (buff[i] == '>') //! > redirection check
        {
            if (i != 0)
            {
                stdoutFlag = i;
                //!Clear buff of the redirect
                buff[i] = 0;     //! Gets rid of '>'
                buff[i + 1] = 0; //!Gets rid of the space ' '
                j = i + 2;
                k = 0;
                //* Do the string copy here & clean buff string
                while (buff[j] != '\0')
                {
                    redirectName[k++] = buff[j];
                    buff[j++] = 0; //!delete the rest of none command chars
                }
                redirectName[k] = 0;
            }
        }
        else if (buff[i] == '<') //! < redirection check
        {
            if (i != 0)
            {
                stdinFlag = i; //! Can use i to know where to split the str
                //!Clear buff of the redirect
                buff[i] = 0;     //! Gets rid of '>'
                buff[i + 1] = 0; //!Gets rid of the space ' '
                j = i + 2;
                int k = 0;
                //* Do the string copy here & clean buff string
                while (buff[j] != '\0')
                {
                    redirectName[k++] = buff[j];
                    buff[j++] = 0; //!delete the rest of none command chars
                }
                redirectName[k] = 0;
            }
        }
        i++;
    }
    i = 0;

    if(DEBUG)
        printf("argcounter %d\n", argcounter);
        printf("buff=%s\n", buff);
        getc();
  
    //! Tokenize command and parameters
    memset(command, 0, sizeof(command));

    strcpy(command, strtok(buff, " "));
    //* Put the command as the first string in myargv
    myargv[i] = command;
    i++;

    if (argcounter > 0)
    {
        while (i <= argcounter)
        {
            myargv[i] = strtok(NULL, " ");
            i++;
        }
        i = 0;
    }


    i = 0; //* Reset counter

    //! Attempt running exec with appending a path each name
    char tempPath[64] = {0};
    i++;

    int r = -1;
    int status = 0;

    printf("Prior to exec tempPath %s\n", command);  // TODO: NEW CHANGE Sat 4-25

    while (1)
    {
        //!=========== IO REDIRECTION ==================================
        int fd = 0;

        if (stdinFlag > 0) // Split the command after <, >, or >>
        {
            printf("< Redirection\n") ;
            close(0); //! Close file descriptor 1, stdin
            fd = open(redirectName, O_RDONLY);
        }
        else if (stdoutFlag > 0) //Split command from i forward
        {
            printf("> Redirection\n") ;
            close(1); //! Close file descriptor 1, stdout
            fd = open(redirectName, O_WRONLY | O_CREAT);
        }
        else if (stdoutAppen > 0) //Split command from i forward
        {
            printf(">> Redirection\n") ;
            close(1); //! Close file descriptor 1, stdout
            fd = open(redirectName, O_RDWR | O_APPEND);
        }
        //!=============== END IO REDIRECTION ==========================

        //strcat(tempPath, command); //! concat tempPath and command
        //printf("Prior to exec tempPath %s\n", tempPath);
        
        int j = 0;
        int k = 0;

        //! Check for shell file, otherwise try to run command
        char fileCheckBuff[8];
        //FILE *fp = fopen(tempPath, "r");
        int fp = open(tempPath, O_RDONLY);
        if (fp != NULL)
        {
            //fread(fileCheckBuff, sizeof(char), 4, fp);
            read(fp, fileCheckBuff, 8);
            if (memcmp(fileCheckBuff, "!#/b", 4) == 0)
            {
                exec("/bin/sh");
            }
            else
            {
                //r = exec(tempPath);
                if(argcounter) 
                {
                    r = exec(tempArg);
                } else {
                    r = exec(command); // TODO: NEW CHANGE Sat 4-25
                }
            }
        }
        printf("After exec tempPath %s\n", tempPath);
        memset(tempPath, 0, sizeof(tempPath)); // *RESET command
        strcpy(tempPath, pathNames[++i]);
        strcat(tempPath, "/");
        strcat(tempPath, command); //! concat tempPath and command
        printf("After new tempPath: %s\n", tempPath);
        myargv[i] = strtok(NULL, " ");
    }
  
    i = 0; //* Reset counter
    //! RESET Redirection
    //stdinFlag = 0;
    //stdoutAppen = 0;
    //stdoutFlag = 0;
    exit(100);
}

int pipeCheck(char buff[])
{
    int pipeFlag = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    char nextBuff[256] = {0};

    //* If pipe is found:
    //! Parse the first buff
    //! Create a pipe
    //!

    //! While the buff is not NULL
    while (buff[i] != '\0')
    {
        if (buff[i] == '|')
        {
            pipeFlag = 1;
            //* Copy rest of string to pass to the next pipe
            //* break and create pipe, else run the exec function passing in original buff

            //!Clear buff of the pipe
            buff[i - 1] = 0; //! clear space before the pipe
            buff[i] = 0;     //! Gets rid of '|'
            buff[i + 1] = 0; //!Gets rid of the space ' ' after the pipe
            
            j = i + 2;
            
            k = 0;
            //* Do the string copy here & clean buff string
            while (buff[j] != '\0')
            {
                nextBuff[k] = buff[j];
                buff[j++] = 0; //!delete the rest of none command chars
                k++;
            }
            //* We could strtok until | for the buff until then
        }
        i++;
    }

    if (pipeFlag != 0) //! Create pipe
    {
        int pd[2] = {0};
        int n = 0;
        i = 0;
        int pid = 0;
        int status = 0;

        //! Pipe creaiton using pipe()
        pipe(pd);
        printf("pd=[%d, %d]\n", pd[0], pd[1]);
        pid = fork();

        printf(">>>>>> INSIDE PIPE CREATION\n");

        if (pid)
        {
            // fork a child to share the pipe
            //printf("parent %d close pd[1]\n", getpid());
            //!Execute using current cleaned up buff / "command"
            //printf("\n Inside parent pipe, command to be ran is buff=%s\n", buff);

            // For combo#1
            //int saved_stdout = dup(0); //!Saved stdout file descriptor
            //close(0);

            //!Combo #1
            close(pd[0]); //Writer close pd[]0 //Write as as the parent close the reader
            close(1);
            dup(pd[1]);
            close(pd[1]);
            
            executeCommand(buff); 
            
            
            int status = 0;
            pid = wait(&status);

            printf("\nChild process with %d exit(%d)\n", pid, status);
            
        }
        else //pid == 0
        {
            //printf("child %d close pd[1]\n", getpid());
            //printf("\n Inside child pipe, command to be ran is nextBuff=%s\n", nextBuff);

            //! Combo #1
            close(pd[1]);
            close(0);
            dup(pd[0]);
            close(pd[0]);
            
            pipeCheck(nextBuff);
            
            exit(100);
        }
    }
    else
    {
        printf("In pipecheck, prior to executeCommand, buff=%s  and ", buff);
        //! Up to here we are good!
        executeCommand(buff);
    }
    exec("sh");
}
