clear
# Start making stuff
echo "Compiling file(s)..."
# krnxxx.o are from the kernel folder
# drivxxx.o are for drivers.
nasm -f elf32 src/kernel/krnentry.asm -o krnent.o
gcc -m32 -c src/kernel/kernel.c -o krnc.o
gcc -m32 -c src/drivers/ports.c -o drivport.o
gcc -m32 -c src/drivers/idt.c -o drividt.o
gcc -m32 -c src/drivers/utils.c -o driv_utils.o
gcc -m32 -c src/drivers/vga.c -o driv_vga.o
ld -m elf_i386 -T src/linker.ld --allow-multiple-definition -o kernel krnent.o krnc.o drivport.o drividt.o driv_utils.o driv_vga.o
# Add some code for when the rest of the code is ready ;-)
echo "Booting into QEMU"
qemu-system-x86_64 -kernel kernel
