# compile-link
arm-none-eabi-as -mcpu=arm926ej-s -g ts.s -o ts.o
arm-none-eabi-gcc -c -mcpu=arm926ej-s -g t.c -o t.o
arm-none-eabi-ld -T t.ld ts.o t.o -o t.elf -L /usr/lib/gcc/arm-none-eabi/6.3.1/ -lgcc 
arm-none-eabi-objcopy -O binary t.elf t.bin
#arm-none-eabi-readelf -a t.elf

rm *.o *.elf

echo ready to go?
read dummy

qemu-system-arm -M versatilepb -m 128M -kernel t.bin -serial mon:stdio