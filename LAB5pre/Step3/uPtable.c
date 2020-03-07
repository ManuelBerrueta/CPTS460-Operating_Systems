//!Dervive from this code, the code for the uPtable()

/* int mkPtable()
{
    int i;
    u32 *ut = (u32 *)0x400000; // at 4MB   //! Ptable reference Page 181
    u32 entry = 0 | 0x412;     // 0x412;// AP=01 (Kmode R|W; Umode NO) domaian=0

    for (i = 0; i < 4096; i++) // clear 4k entries of table
        ut[i] = 0;

    // 128 MB RAM
    for (i = 0; i < 128; i++)
    {
        ut[i] = entry;
        entry += 0x100000;
    }
    // 2MB I/O space at 256 MB
    entry = 256 * 0x100000 | 0x412;
    for (i = 256; i < 258; i++)
    {
        ut[i] = entry;
        entry += 0x100000;
    }
} */

int uPtable(PROC *p)
{
    int i = 0;
    u32 entry = 0 | 0x412;     // 0x412;// AP=01 (Kmode R|W; Umode NO) domaian=0

    p->pgdir = 0x400000 + ((p->pid) * 0x4000); //0x4000 = 16KB (16384)

    for (i = 0; i < 4096; i++) // clear 4k entries of table
        p->pgdir[i] = 0;

    // 128 MB RAM
    for (i = 0; i < 128; i++)
    {
        p->pgdir[i] = entry;
        entry += 0x100000;
    }
    // 2MB I/O space at 256 MB
    entry = 256 * 0x100000 | 0x412;
    for (i = 256; i < 258; i++)
    {
        p->pgdir[i] = entry;
        entry += 0x100000;
    }
    p->pgdir[2048] = 0x800000 + (p->pid -1) * 0x100000 | 0xC32;
}