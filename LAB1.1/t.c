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
//#include <stdio.h>
//#include <string.h>

int prints(char *s)
{
    extern _putc(s);
}

int gets(char *s)
{
    extern _getc(s);
}

char ans[64];

_main()
{
    while (1)
    {
        prints("What's your name? ");
        gets(ans);
        prints("\n\r");

        if (ans[0] == 0)
        {
            prints("return to assembly and hang\n\r");
            return;
        }
        prints("Welcome ");
        prints(ans);
        prints("\n\r");
    }
}
