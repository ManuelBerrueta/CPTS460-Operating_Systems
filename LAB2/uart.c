/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#define DR 0x00
#define FR 0x18

#define RXFE 0x10
#define TXFF 0x20

typedef struct uart
{
    char *base;
    int n;
} UART;

UART uart[4];

typedef unsigned int u32;

int BASE = 10;
int BASE_HEX = 16;
int BASE_OCT = 8;

int uart_init()
{
    int i;
    UART *up;

    for (i = 0; i < 4; i++)
    {
        up = &uart[i];
        up->base = (char *)(0x101F1000 + i * 0x1000);
        up->n = i;
    }
    uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}

int ugetc(UART *up)
{
    while (*(up->base + FR) & RXFE)
        ;
    return *(up->base + DR);
}

int uputc(UART *up, char c)
{
    while (*(up->base + FR) & TXFF)
        ;
    *(up->base + DR) = c;
}

int ugets(UART *up, char *s)
{
    while ((*s = (char)ugetc(up)) != '\r')
    {
        uputc(up, *s);
        s++;
    }
    *s = 0;
}

int uprints(UART *up, char *s)
{
    while (*s)
        uputc(up, *s++);
}

//!!!!!!!!!!!!!!!!!!!!!!!! Start of my code for UART printf !!!!!!!!!!!!!!!!!!!!

int rpu(UART *up, u32 x)
{
    char c;
    if (x)
    {
        c = tab[x % BASE];
        rpu(up, x / BASE);
        uputc(up, c);
    }
}

int printu(UART *up, u32 x)
{
    //? If x==0 then run putchar('0'), else run rpu(x)
    (x == 0) ? uputc(up, '0') : rpu(up, x);
    uputc(up, ' ');
}

int printd(UART *up, int x)
{
    (x == 0) ? uputc(up, '0') : rpu(up, x);
    uputc(up, ' ');
}

int hex(UART *up, u32 x)
{
    char c;
    if (x)
    {
        c = tab[x % BASE_HEX];
        hex(up, x / BASE_HEX);
        uputc(up, c);
    }
}

int printx(UART *up, u32 x)
{
    (x == 0) ? uputc(up, '0') : hex(up, x);
    uputc(up, ' ');
}

int oct(UART *up, u32 x)
{
    char c;
    if (x)
    {
        c = tab[x % BASE_OCT];
        oct(up, x / BASE_OCT);
        uputc(up, c);
    }
}

int printo(UART *up, u32 x)
{
    (x == 0) ? uputc(up, '0') : oct(up, x);
    uputc(up, ' ');
}

int ufprintf(UART *up, char *fmt, ...)
{
    int i = 0;
    //int fmtSize = strlen(fmt);
    int negcheck = 0;

    //cp = (int*)fmt; //!worked

    char *cp;
    int *ip;
    cp = fmt;  //* cp is used to traverse the format string
    ip = (int *)&fmt; //* ip = the address of fmt to traverse up the stack of the parameters passed
    //ip = (int *)&fmt + 1;

    //! Traversing up the stack to the parameters passed to myprintf
    char *temp = cp;

    while (cp)
    {
        //i++;
        if (*cp == '\n')
        {
            uputc(up, '\n');
            uputc(up,'\r');
            cp++;
        }
        else if (*cp != '%')
        {
            uputc(up, *cp);
            cp++;
        }
        else
        {
            i++;
            cp++; //! Move to the next char after
            switch ((int)*cp)
            {
            case 'c': //? char
                uputc(up,*(++ip));
                uputc(up, ' ');
                cp++;
                break;
            case 's': //? string
                uprints(up, (char *)(*(++ip)));
                cp++;
                break;
            case 'u': //? unsigned integer
                printu(up, *(++ip));
                cp++;
                break;
            case 'd': //? integer
                negcheck = (*(++ip));
                if (negcheck < 0)
                {
                    uputc(up, '-');
                    negcheck = negcheck * -1;
                }
                printd(up, negcheck);
                cp++;
                break;
            case 'o': //? unsigned integer in OCT
                printo(up, *(++ip));
                cp++;
                break;
            case 'x': //? unsigned integer in HEX
                printx(up, *(++ip));
                cp++;
                break;

            default:
                break;
            }
        }
    }
}

int mystrlen(char *str)
{
    int i = 0;

    while(str[i] != 0)
    {
        i++;
    }

    return i;
}

int mystrcmp(char *left, char *right)
{ 
    int i = 0;

    if (mystrlen(left) != mystrlen(right))
    {
        return 1;
    }

    
    while (left[i] != 0)
    {
        if (left[i] != right[i])
        {
            return 1;
        }
        i++;
    }

    return 0;
}