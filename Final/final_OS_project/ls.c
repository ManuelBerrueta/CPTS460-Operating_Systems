// test.c file

#include "ucode.c"


char gpath[256];


int tokenize(char *pathname)
{
    // tokenize pathname in GLOBAL gpath[]; pointer by name[i]; n tokens
    int numOfComponents =0;
    strcpy(gpath, pathname);
    
    if(pathname == 0)
    {
        printf("NULL STRING - Can't Tokenize Empty String!\n\n");
        return -1;
    }

    if( strcmp(gpath, "/") == 0)
    {
        printf("Looking to LS root inode\n");
        name[numOfComponents++] = gpath;
        return;
    }

    char* tempPath;
    name[numOfComponents++] = strtok(pathname, "/");

    while((tempPath = strtok(NULL, "/")))
    {
        name[numOfComponents++] = tempPath;
    }

    return numOfComponents;
}


INODE *path2inode(INODE *ip, char pathName[], int inodes_start)
{
    int i = 0;
    int numOfComponents = 0;

    if (pathName[0] != 0) //* If a pathname was passed
    {
        //* Tokenize path
        numOfComponents = tokenize(pathName);
        int j = 0;

        //TODO:For debugging only
        printf("Tokenized path:> ");
        while (j < numOfComponents)
        {
            printf("/%s", name[j++]);
            fflush(stdout);
        }
        puts("");
        //TODO: Search will go here --!NOTE: may need to do ip
        int ino, blk, offset;
        int n = numOfComponents;

        for (i = 0; i < n; i++)
        {
            ino = search(ip, name[i]);

            if (ino == 0)
            {
                printf("can't find %s\n", name[i]);
                exit(1);
            }
            // Mailman's algorithm: Convert (dev, ino) to INODE pointer
            blk = (ino - 1) / 8 + inodes_start;
            offset = (ino - 1) % 8;
            get_block(dev, blk, buf);
            ip = (INODE *)buf + offset; // ip -> new INODE
        }
        //*Print information out of current ip
        printIPinfo(name[i - 1], ip);
        show_dir(ip);
        return ip;
    }
    else
    {
        //show_dir(ip);
        show_dir(running->cwd);
    }
}


int list_file(char *pathname, int inode_start)
{
    printf("list_file()\n");

    root->INODE.i_block[12]; //This is how you access inodes!
    //show_dir(&(root->INODE));
    path2inode(&(root->INODE), pathname, inode_start);
}


int main(int argc, char *argv[])
{
    int i, fd, n;
    char buf[1024];

    printf("ls: this is a test\n");

    printf("argc = %d\n", argc);
    for (i = 0; i < argc; i++)
        printf("argv[%d] = %s\n", i, argv[i]);

}
