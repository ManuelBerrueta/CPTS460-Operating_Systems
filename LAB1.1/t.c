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

char ans[64] = { 0 };

main()
{
    while (1)
    {
        prints("What's your name? ");
        *ans = *ans ^ *ans;
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

int prints(char *s)
{
    while(*s){
        putc(*s);
        s++;
    }
    
    return 1;
}

int gets(char *s)
{
    /* While temp does not equal '\r'*/
    while((*s = getc()) != 0x0D){
        putc(*s);
        s++;
    }
    s++;
    *s = 0;

    return 1;
}
