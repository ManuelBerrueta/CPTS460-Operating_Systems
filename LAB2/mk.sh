#Needed to include library path
LIBPATH=/usr/lib/gcc/arm-none-eabi/6.3.1/

# 1. What does the following statement do? 
#    Assembles ts.s to ts.o targeting the arm926ej-s CPU
arm-none-eabi-as -mcpu=arm926ej-s -g ts.s -o ts.o

# 2. What does the following statement do?
#    Compiles the t.c to t.o targeting the arm926ej-s CPU
arm-none-eabi-gcc -c -mcpu=arm926ej-s -g t.c -o t.o

# 3. What does the following statement do?
#    Links the ts.o & t.o to the t.elf file
#       The ld linker script is used to specify the entry point and layout of
#        the program sections
arm-none-eabi-ld -T t.ld ts.o t.o -o t.elf

# 4. What does the following statement do?
#    Copies and translates the t.elf file into a binary t.bin
#    More explicitly from the man page:
#     objcopy can be used to generate a raw binary file by using an output 
#     target of binary (e.g., use -O binary).  When objcopy generates a 
#     raw binary file, it will essentially produce a memory dump of the contents
#     of the input object file.  All symbols and relocation information  will be
#     discarded.  The memory dump will start at the load address of the lowest
#     section copied into the output file.
arm-none-eabi-objcopy -O binary t.elf t.bin

rm *.o *.elf

echo ready to go?

read dummy

qemu-system-arm -M versatilepb -m 128M -kernel t.bin \
-serial mon:stdio
 
 
 



 





