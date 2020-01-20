/********************************  t.c file  ***********************************
**         CptS 460 - Operating Systems and Computer Architecture             **
**                                 LAB #1                                     **
**                              Boot Program                                  **
**                                  by                                        **
**                            Manuel Berrueta                                 **
**                                                                            **
**    Prereqs:  Ubuntu 18.04 or below                                         **
**              sudo apt-get install build-essential gdb                      **
**              sudo apt-get install gcc-multilib                             **
**                                                                            **
**              (1). Intel x86 packages:                                      **
**                   sudo apt-get install bcc                                 **
**                   sudo apt-get insatll qemu-system-i386                    **
**                                                                            **
**              (2). ARM packages:                                            **
**                   sudo apt-get insatll gcc-arm-none-eabi                   **
**                   sudo apt-get insatll qemu-system-arm                     **
**                                                                            **
**    To build:                                                               **
**              gcc -m32 flag is required                                     **
**                                                                            **
*******************************************************************************/
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"

typedef struct ext2_group_desc GD;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

GD *gp;
INODE *ip;
DIR *dp;

u16 NSEC = 2;
char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;
u16  i, iblk;


int prints(char *s)
{
    while(*s){
        putc(*s);
        s++;
    }
    
    //return 1; Save some bytes
}


int gets(char *s)
{
    /* While temp does not equal '\r' == 0x0D*/
    while((*s = getc()) != 0x0D){
        putc(*s);
        s++;
    }
    s++;
    *s = 0;

    //return 1; Save some bytes
}


u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
    //readfd(blk / 18, ((blk) % 18) / 9, (((blk) % 18) % 9) << 1, buf);
}


u16 search(INODE *ip, char *fname)
{
    for(i=0; i < 12; i++)
    {
        if ((u16)ip->i_block[i] == 0)
        {
            break;
        }
        getblk((u16)ip->i_block[i], buf2);
        dp = (DIR *)buf2;

        // BLK = Block Size = 1024
        while((char *)dp < buf2 + BLK)
        {
            //strncpy(temp, dp->name, dp->name_len);
            //temp[dp->name_len] = 0; //add null char to the end off the dp->name
            dp->name[dp->name_len] = 0;

            //TODO: strcmp to see if the given name exists
            //if (strcmp(fname, temp) == 0)
            if (strcmp(fname, dp->name) == 0)
            {
                //prints("found ino for ");
                prints(fname);
                putc('/');
                //prints("\n\r");
                return (u16)dp->inode;
            }
            dp = (char *)dp + dp->rec_len;
        }
    }

    return 0;
}


/* void print_root(INODE *ip)
{
    for(i=0; i < 12; i++)
    {
        if ((u16)ip->i_block[i] == 0)
        {
            break;
        }
        getblk((u16)ip->i_block[i], buf2);
        dp = (DIR *)buf2;

        // BLK = Block Size = 1024
        while((char *)dp < buf2 + BLK)
        {
            //strncpy(temp, dp->name, dp->name_len);
            //temp[dp->name_len] = 0; //add null char to the end off the dp->name
            dp->name[dp->name_len] = 0;

            prints(dp->name);
            prints("\n\r");

            dp = (char *)dp + dp->rec_len;
        }
    }
    //return 1; //T This 'return 1' takes additional 4 bytes
} */


main()
{
    u32 *up;

    getblk(2, buf1);

    //! 1. Code to get iblk = bg_inode_table block number
    gp = (GD *)buf1;                    //Group Descriptor
    iblk = (u16)gp->bg_inode_table;     //Start of inode table


    //! 2. Code to get root inode
    //Load the INODES from the iblk into buf1
    getblk(iblk, buf1);
    ip = (INODE *)buf1; //Cast the start of buf1 is an INODE
    ip++; //INODE #2 == root


    //! 3. Code to get to the inode for the Kernel
    prints("MB Booter Data: /");  
    //print_root(ip);  //Print root directory
    ino = search(ip, "boot") - 1; //Searching for "boot" in root and get it's inode #
    getblk(iblk+(ino/8), buf2);
    ip = (INODE *)buf2 + (ino % 8); // ip points to boot's inode

    ino = search(ip, "mtx") -1; //Searching for "mtx" in boot directory and get it's inode #
    getblk(iblk+(ino/8), buf2);
    ip = (INODE *)buf2 + (ino % 8); //ip points to mtx's inode
    prints("\n\r");
    
/*     prints("Boot ino=");
    putc(ino + '0'); //Prints out '=' for the ino #...why?
    prints("\n\r");
    prints("returned"); */


    if ((u16)ip->i_block[12])
    {
        //prints("i_blk[12]");
        getblk((u16)ip->i_block[12], buf1);
    }
    
    //! Start loading MTX Kernel loading segment at memory address 0x1000
    setes(0x1000);                      //! ES = 0x1000 => BIOS loads disk block to (ES, buf)

    for(i = 0; i < 12; i++)
    {
        getblk((u16)ip->i_block[i], 0); //! buf = 0 => memory address = (ES, 0)
        inces();                        //! inc ES by 1KB/16 = 0x40 
        putc('*');
    }

    if ((u16)ip->i_block[12])
    {
        up = (u32 *)buf1;

        while(*up)
        {
            getblk((u16)*up, 0);
            inces();
            putc('.');
            up++;
        }
    }

    prints("go?");
    getc();
}
