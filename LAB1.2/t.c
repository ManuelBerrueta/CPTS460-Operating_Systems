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

char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;

int prints(char *s)
{
    extern _putc(s);
}

int gets(char *s)
{
    extern _getc(s);
}

int getblk(u16 blk, char *buf)
{
    // readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
    readfd(blk / 18, ((blk) % 18) / 9, (((blk) % 18) % 9) << 1, buf);
}

main()
{
    u16 i, iblk;
    char c, temp[64];

    prints("TESTING\n");

    prints("read block# 2 (GD)\n\r");
    getblk(2, buf1);

    // 1. WRITE YOUR CODE to get iblk = bg_inode_table block number
    prints("inode_block=");
    putc(iblk + '0');
    prints("\n\r");

    // 2. WRITE YOUR CODE to get root inode
    prints("read inodes begin block to get root inode\n\r");

    // 3. WRITE YOUR CODE to step through the data block of root inode
    prints("read data block of root DIR\n\r");

    // 4. print file names in the root directory /
}
