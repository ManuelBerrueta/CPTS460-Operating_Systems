// kbd.c file

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK 0x0C
#define KISTA 0x10

#include "keymap"
#include "keymap2"

typedef struct kbd
{
    char *base;
    char buf[128];
    int head, tail, data, room;
} KBD;

KBD kbd;
int release;
int keyset;
int shiftKey = 0;
int ctrlKey = 0;
int ctrl_c = 0;


int kbd_init()
{
    char scode;
    keyset = 1; // default to scan code set-1

    KBD *kp = &kbd;
    kp->base = (char *)0x10006000;
    *(kp->base + KCNTL) = 0x10; // bit4=Enable bit0=INT on
    *(kp->base + KCLK) = 8;
    kp->head = kp->tail = 0;
    kp->data = 0;
    kp->room = 128;

    release = 0;

    printf("Detect KBD scan code: press the ENTER key : ");
    while ((*(kp->base + KSTAT) & 0x10) == 0)
        ;
    scode = *(kp->base + KDATA);
    printf("scode=%x ", scode);
    if (scode == 0x5A)
        keyset = 2;
    printf("keyset=%d\n", keyset);
}

// kbd_handler1() for scan code set 1
void kbd_handler1()
{
    u8 scode, c;
    KBD *kp = &kbd;

    scode = *(kp->base + KDATA);

    if (scode & 0x80)
        return;

    c = unsh[scode];
    if (c == '\r')
        kputc('\n');
    kputc(c);

    kp->buf[kp->head++] = c;
    kp->head %= 128;
    kp->data++;
    kp->room--;
}

// kbd_handelr2() for scan code set 2
void kbd_handler2()
{
    u8 scode, c;
    KBD *kp = &kbd;

    char kbd_temp[8];
    scode = *(kp->base + KDATA);

    if (scode == 0xF0)
    { // key release
        release = 1;
        return;
    }

    if (release)
    {
        release = 0;
        return;
    }

    //* Check scan scodes:
    //Left Control Key check
    if (scode == 0x14)
    {
        ctrlKey = 1;
    }

    // Left Shift Key Check
    if (scode == 0x12)
    {
        shiftKey = 1;
    }

    // if realease left control shift key
    if (scode == 0x14 && release)
    {
        //ctrl_d = 0;
        ctrl_c = 0; //Flag for ctrl+c to signal close program
        ctrlKey = 0;
        release = 0;

        return;
    }
    else if (scode == 0x12 && release) //if Left Shift Key release
    {
        shiftKey = 0;
        release = 0;

        return;
    }
    else if (release)
    {
        release = 0;

        return;
    }

    
    if (scode == 0x23 && ctrlKey)
    {
        c = 0x04; //Set to EOF

        kprintf("CTRL + D: %x \n", c);

        kgets(kbd_temp);
    }

    if (scode == 0x21 && ctrlKey)
    {
        kprintf("CTRL + C \n"); //Trying to kill program
        kgets(kbd_temp);

        return;
    }

    if (shiftKey)
    {
        c = utab[scode];
    }
    else
    {
        c = ltab[scode];
    }        

    //printf("kbd interrupt: c=%x %c\n", c, c);
    //kgets(kbd_temp);
    
    //c = ltab[scode];
    printf("%c", c);

    kp->buf[kp->head++] = c;
    kp->head %= 128;
    kp->data++;
    kp->room--;
}

/* //ORIGINAL kbd_handler2()
void kbd_handler2()
{
  u8 scode, c;
  KBD *kp = &kbd;

  scode = *(kp->base + KDATA);

  if (scode == 0xF0){ // key release 
    release = 1;
    return;
  }
  
  if (release){
    release = 0;
    return;
  }

  c = ltab[scode];
  printf("%c", c);

  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++; kp->room--;

} */

void kbd_handler()
{
    if (keyset == 1)
        kbd_handler1();
    else
        kbd_handler2();
}

int kgetc()
{
    char c;
    KBD *kp = &kbd;

    while (kp->data == 0)
        ; // BUSY WAIT for kp>data

    lock();
    c = kp->buf[kp->tail++];
    kp->tail %= 128;
    kp->data--;
    kp->room++;
    unlock();
    return c;
}

int kgets(char s[])
{
    char c;
    while ((c = kgetc()) != '\r')
    {
        if (c == '\b')
        {
            s--;
            continue;
        }
        *s++ = c;
    }
    *s = 0;
    return strlen(s);
}
