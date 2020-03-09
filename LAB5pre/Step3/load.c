#define BLK 1024

extern int getblock(int blk, char *buf);

/* int search(INODE *ip, char *fname)
{
    int i = 0;
    char buf1[BLK], buf2[BLK];
    DIR *dp;

    printf("Searching for %s ino\n", fname);

    for(i=0; i < 12; i++)
    {
        if (ip->i_block[i] == 0)
        {
            break;
        }
        getblock(ip->i_block[i], buf2);
        dp = (DIR *)buf2;

        // BLK = Block Size = 1024
        while((char *)dp < buf2 + BLK)
        {
            //strncpy(temp, dp->name, dp->name_len);
            //temp[dp->name_len] = 0; //add null char to the end off the dp->name
            dp->name[dp->name_len] = 0;
            printf(dp->name);
            //kputc(' ');
            //printf("\n\r");

            //TODO: strcmp to see if the given name exists
            //if (strcmp(fname, temp) == 0)
            if (strcmp(fname, dp->name) == 0)
            {
                printf("found ino for ");
                printf(fname);
                //kputc('/');
                printf("\n\r");
                return dp->inode;
            }
            dp = (char *)dp + dp->rec_len;
        }
    }
    return 0;
} */

int search(INODE *ip, char *name)
{
    // YOUR search() fucntion as in LAB 6
    char temp[256];
    char sbuf[BLK];
    DIR *dp;
    char *cp;
    int ino, block, offset;
    int i=0;

    printf("Searching for %s ino\n", name);

    for(i=0; i < 12; i++)
    {
        if (ip->i_block[i] == 0)
        {
            break;
        }
        getblock(ip->i_block[i], sbuf);
        dp = (DIR *)sbuf;
        cp = sbuf;

        while(cp < sbuf + BLK)
        {
            strcpy(temp, dp->name);
            temp[dp->name_len] = 0; //add null char to the end off the dp->name

            //TODO: strcmp to see if the given name exists
            if (strcmp(&name[i], temp) == 0)
            {
                printf("*****={ inode %s found, inode# = %d }=*****\n", name, dp->inode);
                printf("%4d       %4d      %4d        %s\n", 
                dp->inode, dp->rec_len, dp->name_len, temp);
                return dp->inode;
            }
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    }
    printf("**inode %s, not found in data blocks\n\n", name);
    return 0;
}


int load(char *filename, PROC *p)
{
    GD *gp;
    INODE *ip;
    DIR *dp;
    int *up;

    char buf1[BLK], buf2[BLK];
    int ino;
    int  i, iblk;

    
    // Load Group Descriptor Block (Block #2)
    getblock(2, buf1);
    //! Code to get iblk = bg_inode_table block number
    gp = (GD *)buf1;               //Group Descriptor cast
    iblk = gp->bg_inode_table;     //Start of inode table (i_block with inodes)



        //! Code to get root inode
    // Load the i_block that contains the INODES into buf1, iblk is the block# that contains them
    getblock(iblk, buf1);
    ip = (INODE *)buf1; //Cast the start of buf1 is an INODE
    ip++; //INODE #2 == root


    //! Code to get to the inode for the file
    printf("\nMB Loader ::> %s\n", filename);
    printf("\n\r");
    //print_root(ip);  //Print root directory
    
    // TODO: Currently broken here, possibly bad search function
    
    printf("=={ Searching for bin ino }==\n");
    ino = search(ip, "bin") - 1; //Searching for "bin" in root and get it's inode #
    //printf("\n\r");
    getblock(iblk+(ino/8), buf2);
    ip = (INODE *)buf2 + (ino % 8); // ip points to bin's inode

    printf("=={ Searching for %s ino }==\n", filename);
    ino = search(ip, filename) -1; //Searching for filename in bin directory and get it's inode #
    getblock(iblk+(ino/8), buf2);
    ip = (INODE *)buf2 + (ino % 8); //ip points to filename's inode
    //printf("\n\r");
    

    //! Check for indirect iblock, if they exist load into buf1
    if (ip->i_block[12])
    {
        getblock(ip->i_block[12], buf1);
    }


    //TODO: I believe the issue is here, need to compensate for the address of the umode image
    //* Because we are loading this binaries into the umode area, we must load at that adddress
    char *umode_mem_address = (char *) 0x800000 + (p->pid -1) * 0x100000; //| 0xC32;
    

    
    //! Start loading filename image segment
    //* Load first 12 blocks
    for(i = 0; i < 12; i++)
    {
        //getblock(ip->i_block[i], 0); //! buf = 0 => memory address = (ES, 0)
        getblock(ip->i_block[i], umode_mem_address); //! buf = 0 => memory address = (ES, 0)
        umode_mem_address += 1024;
        kputc('*');
    }
    printf("\n\r");

    //* Load indirect blocks if any
    if (ip->i_block[12])
    {
        up = buf1;

        while(*up)
        {
            //getblock(*up, 0);
            getblock(*up, umode_mem_address);
            kputc('.');
            up++;
            umode_mem_address += 1024;
        }
    }
    printf("\n\rFinished Loading %s\n", filename);
    getc();
    return 1;
}