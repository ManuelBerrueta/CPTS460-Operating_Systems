#define BLK 1024
extern GD *gp;
extern INODE *ip;
extern DIR *dp;

//extern u16 NSEC = 2;
extern char buf1[BLK], buf2[BLK];
//extern int color = 0x0A;
extern u8 ino;
extern u16  i, iblk;

u16 search(INODE *ip, char *fname)
{
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
}


int load(char *filename, PROC *p)
{
    int * up;
    
    //Load Group Descriptor Block
    getblock(2, filename);

    //! 1. Code to get iblk = bg_inode_table block number
    gp = (GD *)buf1;               //Group Descriptor cast
    iblk = gp->bg_inode_table;     //Start of inode table (i_block with inodes)



    //TODO: START HERE CLEANING UP THIS CODE  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        //! 2. Code to get root inode
    //Load the i_block that contains the INODES into buf1, iblk is the block# that contains them
    getblock(iblk, buf1);
    ip = (INODE *)buf1; //Cast the start of buf1 is an INODE
    ip++; //INODE #2 == root


    //! 3. Code to get to the inode for the Kernel
    printf("\nMB Booter ::>%s\n", filename);
    printf("\n\r");
    //print_root(ip);  //Print root directory
    ino = search(ip, "bin") - 1; //Searching for "boot" in root and get it's inode #
    //printf("\n\r");
    getblock(iblk+(ino/8), buf2);
    ip = (INODE *)buf2 + (ino % 8); // ip points to boot's inode

    ino = search(ip, filename) -1; //Searching for "mtx" in boot directory and get it's inode #
    getblock(iblk+(ino/8), buf2);
    ip = (INODE *)buf2 + (ino % 8); //ip points to mtx's inode
    //printf("\n\r");
    

    if (ip->i_block[12])
    {
        getblock(ip->i_block[12], buf1);
    }

    
    //! 4. Set the memory address to 0x1000 to start loading the MTX Kernel
    //setes(0x1000);                      //! ES = 0x1000 => BIOS loads disk block to (ES, buf)

    
    //! 5. Start filename image loading segment
    //* Load first 12 blocks
    for(i = 0; i < 12; i++)
    {
        getblock(ip->i_block[i], 0); //! buf = 0 => memory address = (ES, 0)
        //inces();                        //! inc ES by 1KB/16 = 0x40 
        kputc('*');
    }
    printf("\n\r");

    //* Load indirect blocks if any
    if (ip->i_block[12])
    {
        up = (u32 *)buf1;

        while(*up)
        {
            getblock(*up, 0);
            //inces();
            kputc('.');
            up++;
        }
    }
    printf("\n\rFIN");

    //printf("FIN");
    getc();
}