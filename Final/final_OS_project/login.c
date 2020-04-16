// login.c file

#include "ucode.c"

int in, out, err;
char uname[128], password[128], *eachword[64];


void pwd_token(char *line)
{
  char *cp;
  cp = line;
  int i = 0;
  
  while (*cp != 0){
       while (*cp == ':') *cp++ = 0;        
       if (*cp != 0)
           eachword[i++] = cp;         
       while (*cp != ':' && *cp != 0) cp++;                  
       if (*cp != 0)   
           *cp = 0;                   
       else 
            break; 
       cp++;
  }
  eachword[i] = 0;
}

int verifyCreds(int fd, char *name, char *passwd)
{
    char buf[1024];
    char temp_line[512];
    int user_flag = 0;
    int passwd_flag = 0;
    int i, n;
    int j = 0;
    
    n = read(fd, buf, 1024);
    buf[n] = 0;

    for (i=0 ; i < 1024; i++)
    {
        //Check if the next char is null, if so then break
        if (buf[i] == 0)
        {
            break;
        }
        // Split the lines when new line '\n' char is found within the buf
        if (buf[i] == '\n')
        {
            temp_line[i] = 0;

            pwd_token(temp_line);

            if (strcmp(eachword[0], name) == 0)
            {
                user_flag = 1;
            }
            
            if (strcmp(eachword[1], passwd) == 0)
            {
                passwd_flag = 1;
            }

            if (user_flag && passwd_flag)
            {
                printf("Credentials Verified!\n");
                close(fd);
                return 1;
            }
        }
        // copy chars
        temp_line[i] = buf[i];
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int i, fd, n, uid, gid;
    char buf[1024];

    close(0);
    close(1);

    in  = open(argv[1], O_RDONLY);
    out = open(argv[1], O_WRONLY);
    err = open(argv[1], O_WRONLY);
    settty(argv[1]);

    fd = open("/etc/passwd", O_RDONLY);

    if (fd <= 0)
    {
        printf("FAILED to open passwd file!\n\n");

        exit(-1);
    }

    printf("BERRLOGIN: open %s as in=0, out=1, err=2\n",  argv[1]);


    while(1)
    {
        printf("login:> ");
        gets(uname);
        printf("password:> ");
        gets(password);

        n = verifyCreds(fd, uname, password);
        
        if (n)
        {
            printf("BERRLOGIN SUCCESS\n");
            //TODO: login success and execute shell
            uid = atoi(eachword[3]);
            gid = atoi(eachword[2]);
            chuid(uid, gid);
            chdir(eachword[5]);
            printf("Spawning Shell\n");
            exec("sh in out err");
        }
        else
        {
            printf("BERRLOGIN FAILED\nCheck credentials and try again\n");
        }
    }

    close(fd);
}
