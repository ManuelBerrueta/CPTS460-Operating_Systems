#include "ucode.c"
#define printk printf
#define BLKSIZE 1024

//struct stat mystat, *sp = &mystat;
char t1[18] = "xwrxwrxwr-------";
char t2[18] = "----------------";


int ls_file(char *fname)
{
    struct stat fileStat, *sp;
    int r, i;
    char tempName[64];
    char buff[4096];
    
    //printf("inside ls_file fname=%s\n", fname);
    //if ((r = lstat(fname, &fileStat)) < 0)
    if ((r = stat(fname, &fileStat)) < 0)
    {
        printf("can’t stat %s\n", fname);
        exit(1);
    }
    sp = &fileStat;


    if ((sp->st_mode & 0xF000) == 0x8000) // if (S_ISREG())
        //printf("%c",'-');
        //printf("%s",'-');
        //putchar('-');
        mputc('-');
        //printf("TESTINGreg\n");
    if ((sp->st_mode & 0xF000) == 0x4000) // if (S_ISDIR())
        //printf("%c",'d');
        //printf("%s",'d');
        //putchar('d');
        mputc('d');
        //printf("TESTINGdir\n");
    if ((sp->st_mode & 0xF000) == 0xA000) // if (S_ISLNK())
        //printf("%c",'l');
        //printf("%s",'l');
        //putchar('l');
        mputc('l');
        //printf("TESTINGlink\n");
    for (i = 8; i >= 0; i--)
    {
        if (sp->st_mode & (1 << i)) // print r|w|x
        {
            //printf("%c", t1[i]);
            //printf("%s", t1[i]);
            //putchar(t1[i]);
            //putchar('t');
            mputc(t1[i]);
        }
        else
        {
            //printf("%c", t2[i]);
            //printf("%s", t2[i]);
            //putchar(t2[i]);
            //putchar('t');
            mputc(t2[i]);
        }
        // or print -
    }

    //printf("%4d ", sp->st_nlink);   // link count
    //printf("%4d ", sp->st_gid);     // gid
    //printf("%4d ", sp->st_uid);     // uid
    //printf("%8d ", sp->st_size);    // file size
    if(sp->st_nlink > 9)
        printf("  \t%d\t ", sp->st_nlink);   // link count
    else
        printf("  \t %d\t ", sp->st_nlink);   // link count
    printf("%d\t ", sp->st_gid);     // gid
    printf("%d\t ", sp->st_uid);     // uid
    printf("%d\t", sp->st_size);    // file size
    //fflush(stdout);

    // print time
    //strcpy(ftime, ctime(&sp->st_ctime)); // print time in calendar form
    //ftime[strlen(ftime) - 1] = 0;   // kill \n at end
    //printf("%s ", ftime);
    // print name
    /* char tempBasename[64]= "";

    if( (strcmp(fname, ".") == 0) || (strcmp(fname, "..") == 0))
    {   
        printf("%s", fname); // print linked name
    }
    else
    {
        strcpy(tempBasename, fname);
        printf("%s", basename(tempBasename)); // print file basename
    } */
    printf("%s", fname);
    
    // print -> linkname if symbolic file
    if ((sp->st_mode & 0xF000) == 0xA000)
    {
       //TODO: Added this code, was not provided. Needs checking.
       // use readlink() to read linkname
        //readlink(sp, buff, 40); 
        readlink(sp, buff); 
        printf(" -> %s", buff); // print linked name
    }
    printf(" \n");
    //printf("END of ls_file\n");
}


int ls_dir(char *dname) //! From 8.6.5
{
    DIR *dp;
    int dir_fd = 0, r = 0;
    char dirBuf[BLKSIZE] = { 0 }, tempName[255] = { 0 }, path[1024] = { 0 };
    char *cp;


    dir_fd = open(dname, O_RDONLY);

    if (dir_fd < 0)
    {
        printf("ERROR Openinig DIR=%s\n", dname);
        return;
    }

    // Read in block size of 1024
    r = read(dir_fd, dirBuf, BLKSIZE);
    cp = dirBuf;
    dp = (DIR *) dirBuf; //Cast the buffer is a dir struct
    

    while (cp < dirBuf + BLKSIZE)
    {
        strcpy(tempName, dp->name);
        tempName[dp->name_len] = 0; // must finish of the end of the string
        //Build path
        //printf("BUILD PATH\n");
        //strcpy(path, dname);
        //strcat(path, "/");
        //strcat(path, tempName);

        //printf("name=%s ", ep->d_name);
        //TODO: if the path is just / don't come here!
        ls_file(tempName);
        printf("\n");
        cp += dp->rec_len;
        dp = (DIR *)cp;
        //fflush(stdout);
    }
}


int checkForDIR(u16 mode)
{
    printf("Checking for Dir\n");
    if ((mode & 0xF000) == 0x4000)
    {
        return 1;
    }
    return 0;
}


int main(int argc, char *argv[])
{
    int r;
    char *filename, cwd[256];
    struct stat mystat, *sp = &mystat;
    filename = "./";
    
    printf("INSIDE LS\n");
    // default to CWD
    if (argc > 1)
    {
        //filename = argv[1]; // if specified a filename
        strcpy(filename, argv[1]);
    }


    //if (r = lstat(filename, sp) < 0)
    if (r = stat(filename, sp) < 0)
    {
        printf("no such file %s\n", filename);
        exit(1);
    }
    //strcpy(path, filename);
    /* if (path[0] != '/') // Then filename is relative : get CWD path
    { 
        //getcwd(cwd, 256);
        getcwd(cwd);
        strcpy(path, cwd);
        strcat(path, "/");
        strcat(path, filename);
    } */
    
    //if (checkForDIR(sp->st_mode))
    if ((sp->st_mode & 0xF000) == 0x4000)
    {
        ls_dir(filename);
    }
    else
    {
        ls_file(filename);
    }
}
