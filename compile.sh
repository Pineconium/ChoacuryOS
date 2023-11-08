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
ld -m elf_i386 -T src/linker.ld --allow-multiple-definition krnent.o krnc.o drivport.o drividt.o driv_utils.o driv_vga.o -o ChoacuryOS.bin -nostdlib
# Check the binary file is x86 multiboot file or not
echo "Checking if BINARY is GRUB-Ready..."
grub-file --is-x86-multiboot ChoacuryOS.bin
# Building the ISO file
echo "Making ISO file"
mkdir -p isodir/boot/grub
cp ChoacuryOS.bin isodir/boot/ChoacuryOS.bin
cp src/boot/grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o ChoacuryOS.iso isodir
# Add some code for when the rest of the code is ready ;-)
echo "Booting into QEMU"
qemu-system-x86_64 -cdrom ChoacuryOS.iso
