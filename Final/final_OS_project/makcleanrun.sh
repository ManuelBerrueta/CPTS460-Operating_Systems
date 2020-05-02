rm sdimage
cp sdimage.original sdimage

sudo ./mku init
sleep .1
sudo ./mku login
sleep .1
sudo ./mku cat
sleep .1
sudo ./mku l2u
sleep .1
sudo ./mku sh
sleep .1
sudo ./mku more
sleep .1
sudo ./mku ls
sleep .1
sudo ./mku grep
sleep .1
sudo ./mku cp 

qemu-system-arm -M versatilepb -m 256M -sd sdimage -kernel wanix -serial mon:stdio #-serial /dev/pts/2