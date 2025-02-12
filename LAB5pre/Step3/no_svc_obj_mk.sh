# (cd USER; mku u1; mku u2)

arm-none-eabi-as -mcpu=arm926ej-s ts.s -o ts.o
arm-none-eabi-gcc -c -mcpu=arm926ej-s t.c -o t.o
arm-none-eabi-ld -T t.ld ts.o t.o uPtable.obj load.obj -Ttext=0x10000 -o t.elf
arm-none-eabi-objcopy -O binary t.elf t.bin

rm *.o *.elf

echo ready to go?
read dummy

qemu-system-arm -M versatilepb -m 128M -sd sdimage -kernel t.bin
