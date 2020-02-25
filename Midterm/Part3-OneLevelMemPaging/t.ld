/****************
qemu with -kernel file.bin ==> start to execute from 0x10000 ==>
file.bin must be loaded to 0x10000 ==> . = 0x10000
************/
ENTRY(reset_handler)
SECTIONS
{
  . = 0x10000;
  .text : {
      ts.o
      *(.text .rodata)
  }
  .data : { *(.data) }
  .bss : { *(.bss) }
  . = ALIGN(8);
  . = . + 0x1000; /* 4kB of SVC mode stack memory */
  svc_stack_top = .;
  . = . + 0x1000; /* 4kB of irq mode stack memory */
  irq_stack_top = .;
  /* brand new code - Testing */
  . = . + 0x1000; /* 4kB of data abort stack memory */
  und_stack_top = .;
  . = . + 0x1000; /* 4kB of undefine stack memory */
  abt_stack_top = .;
  . = ALIGN(1024);
}

