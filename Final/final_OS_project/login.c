#include "ucode.c"

int in, out, err;
char *token_passwd[16];
char uname[128], password[128], buf[1024];

void tokenize_passwd(char *line)
{
    char *cp;
    cp = line;
    int i = 0;

    while (*cp != 0)
    {
        while (*cp == ':')
            *cp++ = 0;
        if (*cp != 0)
            token_passwd[i++] = cp;
        while (*cp != ':' && *cp != 0)
            cp++;
        if (*cp != 0)
            *cp = 0;
        else
            break;
        cp++;
    }
    token_passwd[i] = 0;
}


int main(int argc, char *argv[])
{
    int i, fd, uid, gid;
    int tmp_line_index;
    int next_line = 0;
    int verified_creds = 0;
    int n = 0;
    int j = 0;
    char temp_line[256];

    // Close any old open file descriptors
    close(0);
    close(1);
    close(2);

    // Open our own file descriptors
    in = open(argv[1], 0);
    out = open(argv[1], 1);
    err = open(argv[1], 2);
    settty(argv[1]);

    fd = open("/etc/passwd", O_RDONLY);

    if (fd <= 0)
    {
        printf("FAILED to open passwd file!\n\n");

        exit(-1);
    }

    n = read(fd, buf, 1024);

    while (1)
    {
        printf("Login:");
        gets(uname);
        printf("Password:");
        gets(password);

        for (i = 0; i < n; i++)
        {
            //Check if the next char is null, if so then break
            if (buf[i] == 0)
            {
                break;
            }
            if (buf[i] == '\n')
            {
                // We copy the current line in passwoprd  to our temp_line
                // We must keep track of the curren position in the buf
                j = 0;
                for (tmp_line_index = next_line; tmp_line_index <= i; tmp_line_index++)
                {
                    temp_line[j] = buf[tmp_line_index];
                    j++;
                }
                temp_line[j] = 0;

                // Tokenize each line in passwd file
                tokenize_passwd(temp_line);
               
                // Compare and verify that both the username and password match
                // If they match we execute the rest of the login process
                // and we execute the shell program
                if ((strcmp(token_passwd[0], uname) == 0) && (strcmp(token_passwd[1], password) == 0))
                {
                    close(fd);
                    
                    printf("BERRLOGIN SUCCESS\n");
                    
                    uid = atoi(token_passwd[3]);
                    gid = atoi(token_passwd[2]);
                    chuid(uid, gid);
                    chdir(token_passwd[5]);
                    printf("Spawning Shell\n");
                    
                    exec("sh in out err");
                
                    break;
                }
                //next_line = i++; //This breaks it for some reason?!
                next_line = i + 1;
            }
        }
        next_line = 0;
    }
}