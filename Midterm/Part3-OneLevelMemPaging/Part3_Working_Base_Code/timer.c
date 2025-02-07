// timer register u32 offsets from base address
#define TLOAD 0x0
#define TVALUE 0x1
#define TCNTL 0x2
#define TINTCLR 0x3
#define TRIS 0x4
#define TMIS 0x5
#define TBGLOAD 0x6

extern int kpchar(char c, int ro, int co);
extern int kprintf(char *fmt, ...);

extern int color;

typedef volatile struct timer
{
    u32 *base;            // timer's base address; as u32 pointer
    int tick, hh, mm, ss; // per timer data area
    char clock[16];
} TIMER;

volatile TIMER timer[4]; //4 timers; 2 per unit; at 0x00 and 0x20


void timer_init()
{
    int i;
    TIMER *tp;
    kprintf("timer_init()\n");
    for (i = 0; i < 4; i++)
    {
        tp = &timer[i];
        if (i == 0)
            tp->base = (u32 *)0x101E2000;
        if (i == 1)
            tp->base = (u32 *)0x101E2020;
        if (i == 2)
            tp->base = (u32 *)0x101E3000;
        if (i == 3)
            tp->base = (u32 *)0x101E3020;
        *(tp->base + TLOAD) = 0x0; // reset
        *(tp->base + TVALUE) = 0xFFFFFFFF;
        *(tp->base + TRIS) = 0x0;
        *(tp->base + TMIS) = 0x0;
        *(tp->base + TLOAD) = 0x100;
        // CntlReg=011-0010=|En|Pe|IntE|-|scal=01|32bit|0=wrap|=0x66
        *(tp->base + TCNTL) = 0x66;
        *(tp->base + TBGLOAD) = 0x1C00;          // timer counter value
        tp->tick = tp->hh = tp->mm = tp->ss = 0; // initialize wall clock
        kstrcpy((char *)tp->clock, "00:00:00");
    }
}


int timer_clearInterrupt(int n) // timer_start(0), 1, etc.
{
    TIMER *tp = &timer[n];
    *(tp->base + TINTCLR) = 0xFFFFFFFF;
}


void timer_handler(int n)
{
    int i;
    TIMER *t = &timer[n];
    t->tick++; // Assume 120 ticks per second
    //if (t->tick == 120)
    if (t->tick == 5) //! NOTE: Here is where you change the speed!
    {
        //printf("At every second Here we decremnt Queue\n");
        //TODO: it may be possible to do all of the code here?
        //! if value of a proc in the queue is 0, then put back in readyQueue

/*         if (sleepList)
        {
            //must check value
            // Here is just decrement the head->priority:
            //     if the head ->priority == 0;
            //         dequeue  by PROC *p = dequeue
            // ! or may be just use wakeup since wakeup dequeue](& wakeup proc by kwakeup()
            sleepList->priority--;
            
            if (sleepList->priority <= 0)
            {
                //timerDequeue
                //This should wakeup the process as well
                //PROC *timeOut = timerDequeue();
                // also add to readyQueue
                PROC *timeOutProc = dequeue(&sleepList);
                
                //kwakeup(&timeOutProc->pid);
                timerWakeup(&timeOutProc->pid);
                printf("\n\n Woke up Proc %d", timeOutProc->pid);

                //TODO: Possibly kfork body if sleepList is empty after dequeue
            }    
            // I can probably use a modified printlist to show the time
            //Print timerqueue
            printTQE(sleepList);
        }
        else
        {
            printf("\n\nTimerQueue is empty\n\n");
        } */
        
        
        t->tick = 0;
        t->ss++;
        if (t->ss == 60)
        {
            t->ss = 0;
            t->mm++;
            if (t->mm == 60)
            {
                t->mm = 0;
                t->hh++; // no 24 hour roll around
            }
        }
        t->clock[7] = '0' + (t->ss % 10);
        t->clock[6] = '0' + (t->ss / 10);
        t->clock[4] = '0' + (t->mm % 10);
        t->clock[3] = '0' + (t->mm / 10);
        t->clock[1] = '0' + (t->hh % 10);
        t->clock[0] = '0' + (t->hh / 10);
    }
    color = n; // display in different color
    for (i = 0; i < 8; i++)
    {
        kpchar(t->clock[i], n, 70 + i); // to line n of LCD
    }
    timer_clearInterrupt(n); // clear timer interrupt
}


void timer_start(int n) // timer_start(0), 1, etc.
{
    TIMER *tp = &timer[n];
    kprintf("timer_start %d base=%x\n", n, tp->base);
    *(tp->base + TCNTL) |= 0x80; // set enable bit 7
}


void timer_stop(int n) // stop a timer
{
    TIMER *tp = &timer[n];
    *(tp->base + TCNTL) &= 0x7F; // clear enable bit 7
}
