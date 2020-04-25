#include "ucode.c"
#define printk printf


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

    //close(0);
    //close(1);

    //stdin  = open(argv[1], O_RDONLY);
    //stdout = open(argv[1], O_WRONLY);
    //err = open(argv[1], O_WRONLY);


    //! Get user input
    while (1)
    {
        if (getcwd(cwd) == 0)
        {
            printf("getcwd Failed!\n\n");
        }

        //printf("[ %04d/%02d/%02d ] BERR Shell [ %s ]\n|-$ ", tm.tm_year + 1900, tm.tm_mon, tm.tm_mday, cwd);
        printf("BERR Shell [ %s ]-$ ", cwd);
        gets(buff);
        //buff[strlen(buff) - 1] = 0; // *Get rid of '\n'
        buff[strlen(buff)] = 0; // *Get rid of '\n'
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
            //TODO: Check for pipes & run command
            pipeCheck(buff);
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
    char *myargv[246] = {0}; //! TODO: MAY NEED TO REPLACE this with one passed in?
    char command[16];        //? Command string

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
                buff[i] = 0;
                buff[i + 1] = 0;
                buff[i + 2] = 0;

                j = i + 3;
                int k = 0;
                //TODO: Do the string copy here & clean buff string
                while (buff[j] != '\0')
                {
                    redirectName[k++] = buff[j];
                    buff[j++] = 0; //!delete the rest of none command chars
                }
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
                //TODO: Do the string copy here & clean buff string
                while (buff[j] != '\0')
                {
                    redirectName[k++] = buff[j];
                    buff[j++] = 0; //!delete the rest of none command chars
                }
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
                //TODO: Do the string copy here & clean buff string
                while (buff[j] != '\0')
                {
                    redirectName[k++] = buff[j];
                    buff[j++] = 0; //!delete the rest of none command chars
                }
            }
        }
        i++;
    }

    i = 0;
    //! Tokenize command and parameters
    memset(command, 0, sizeof(command));
    strcpy(command, strtok(buff, " "));
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

    argcounter = 0;
    //! Count Number Of Paths
    while (path[i] != '\0')
    {
        if (path[i] == ':')
        {
            argcounter++;
        }
        i++;
    }
    argcounter++; //! for the last path without ":"
    i = 0;
    //! Tokenize paths
    char commpath[64];
    pathNames[i] = strtok(path, ":");

    i = 0; //* Reset counter

    //! Attempt running exec with appending a path each name
    char tempPath[64] = {0};
    i++;

    int r = -1;
    int status = 0;

    int pid = fork();
    if (pid)
    {
        printf("\n======> PARENT=%d WAITS for CHILD=%d to DIE <======\n", getpid(), pid);
        pid = wait(&status);
        printf("\n======> DEAD CHILD=%d, STATUS=0x%04x <======\n\n", pid, status);
    }
    else
    {
        printf("%s\n", tempPath);
        // getchar();
        printf("CHILD=%d STARTED | My PARENT=%d\n", getpid(), getppid());

        strcat(tempPath, command); //! concat tempPath and command
        //while (r == -1)
        while (1)
        {
            //!=========== IO REDIRECTION ==================================
            int fd = 0;

            //TODO: Split the command after <, >, or >>
            if (stdinFlag > 0)
            {
                close(0); //! Close file descriptor 1, stdin
                open(redirectName, O_RDONLY);
            }
            else if (stdoutFlag > 0)
            {
                //TODO: Split command from i forward
                close(1); //! Close file descriptor 1, stdout
                open(redirectName, O_WRONLY | O_CREAT);
            }
            else if (stdoutAppen > 0)
            {
                //TODO: Split command from i forward
                close(1); //! Close file descriptor 1, stdout
                open(redirectName, O_RDWR | O_APPEND);
            }
            //!=============== END IO REDIRECTION ==========================

            //strcat(tempPath, command); //! concat tempPath and command
            printf("Prior to exec tempPath %s\n", tempPath);
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
                    r = exec(tempPath);
                }
            }
            printf("After exec tempPath %s\n", tempPath);
            memset(tempPath, 0, sizeof(tempPath)); // *RESET command
            strcpy(tempPath, pathNames[++i]);
            strcat(tempPath, "/");
            strcat(tempPath, command); //! concat tempPath and command
            printf("After new tempPath: %s\n", tempPath);
        }
        exit(100);
    }
    i = 0; //* Reset counter
    //TODO: RESET Redirection
    stdinFlag = 0;
    stdoutAppen = 0;
    stdoutFlag = 0;
}

int pipeCheck(char buff[])
{
    int pipeFlag = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    char nextBuff[256] = {0};

    //TODO: If pipe is found:
    //! Parse the first buff
    //! Create a pipe
    //!

    //! While the buff is not NULL
    while (buff[i] != '\0')
    {
        if (buff[i] == '|') //TODO: HERE I AM
        {
            pipeFlag = 1;
            //TODO: Copy rest of string to pass to the next pipe
            //TODO: break and create pipe, else run the exec function passing in original buff

            //!Clear buff of the pipe
            buff[i] = 0;     //! Gets rid of '|'
            buff[i + 1] = 0; //!Gets rid of the space ' '
            j = i + 2;
            k = 0;
            //TODO: Do the string copy here & clean buff string
            while (buff[j] != '\0')
            {
                nextBuff[k] = buff[j];
                buff[j++] = 0; //!delete the rest of none command chars
                k++;
            }
            //TODO: We could strtok until | for the buff until then
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
            int saved_stdout = dup(1); //!Saved stdout file descriptor

            // fork a child to share the pipe
            printf("parent %d close pd[0]\n", getpid());
            close(pd[0]); //Writer close pd[]0

            dup2(pd[1], 1);
            close(pd[1]);

            //!Execute using current cleaned up buff / "command"
            executeCommand(buff);
            dup2(saved_stdout, 1);
            close(saved_stdout);

            pid = wait(&status);

            printf("parent %d exit\n", getpid());
        }
        else
        {
            printf("child %d close pd[1]\n", getpid());

            int saved_stdin = dup(0);
            close(pd[1]);
            // child as pipe READER
            close(0);
            //fflush(stdout);
            dup2(pd[0], 0);
            close(pd[0]);

            pipeCheck(nextBuff);
            dup2(saved_stdin, 0);
            close(saved_stdin);

            exit(100);
        }
    }
    else
    {
        executeCommand(buff);
    }
}
