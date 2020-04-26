// test.c file

#include "ucode.c"
#define printk printf
/* * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Code from 8.6.7 - The ls Program by K.C. Wang   *
 * from his book Systems Programming in Unix/Linux *
 * * * * * * * * * * * * * * * * * * * * * * * * * */


struct stat mystat, *sp;
char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

int ls_file(char *fname)
{
    struct stat fileStat, *sp;
    int r, i;
    char ftime[64];
    char buff[4096];
    sp = &fileStat;
    //if ((r = lstat(fname, &fileStat)) < 0)
    if ((r = fstat(fname, &fileStat)) < 0)
    {
        printf("can’t stat %s\n", fname);
        exit(1);
    }
    if ((sp->st_mode & 0xF000) == 0x8000) // if (S_ISREG())
        printf("%c",'-');
    if ((sp->st_mode & 0xF000) == 0x4000) // if (S_ISDIR())
        printf("%c",'d');
    if ((sp->st_mode & 0xF000) == 0xA000) // if (S_ISLNK())
        printf("%c",'l');
    for (i = 8; i >= 0; i--)
    {
        if (sp->st_mode & (1 << i)) // print r|w|x
        {
            printf("%c", t1[i]);
        }
        else
        {
            printf("%c", t2[i]);
        }
        // or print -
    }

    printf("%4d ", sp->st_nlink);   // link count
    printf("%4d ", sp->st_gid);     // gid
    printf("%4d ", sp->st_uid);     // uid
    printf("%8d ", sp->st_size);    // file size
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
    printf("%s ", fname);
    
    // print -> linkname if symbolic file
    if ((sp->st_mode & 0xF000) == 0xA000)
    {
       //TODO: Added this code, was not provided. Needs checking.
       // use readlink() to read linkname
        //readlink(sp, buff, 40); 
        readlink(sp, buff); 
        printf(" -> %s", buff); // print linked name
    }
    printf("\n");
}


int ls_dir(char *dname) //! From 8.6.5
{

    struct ext2_dir_entry_2 *ep;
    int dir_fd = 0;
    char dirBuf[1024] = { 0 };

    dir_fd = open(dname, O_RDONLY);
    
    while (ep = read(dir_fd, dirBuf, 1024))
    {
        //printf("name=%s ", ep->d_name);
        //TODO: if the path is just / don't come here!
        //ls_file(ep->d_name);
        //ls_file(dname);
        printf("%s\n", ep->name);
        //fflush(stdout);
    }
}


int S_ISDIR(u16 mode)
{
    if ((mode & 0xF000) == 0x4000)
    {
        return 1;
    }
}


int main(int argc, char *argv[])
{
    struct stat mystat, *sp = &mystat;
    int r;
    char *filename, path[1024], cwd[256];
    filename = "./";
    // default to CWD
    if (argc > 1)
    {
        filename = argv[1]; // if specified a filename
    }
    //if (r = lstat(filename, sp) < 0)
    if (r = stat(filename, sp) < 0)
    {
        printf("no such file %s\n", filename);
        exit(1);
    }
    strcpy(path, filename);
    if (path[0] != '/') // Then filename is relative : get CWD path
    { 
        //getcwd(cwd, 256);
        getcwd(cwd);
        strcpy(path, cwd);
        strcat(path, "/");
        strcat(path, filename);
    }
    if (S_ISDIR(sp->st_mode))
    {
        ls_dir(path);
    }
    else
    {
        ls_file(path);
    }
}
