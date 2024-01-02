#!/bin/bash
DRIV_DIR=src/drivers
KERN_DIR=src/kernel
BUILD=build/
FOLDER_MAIN=src/files

clear
# Make /build/ directory
mkdir -p build
# Start making stuff
echo "Compiling file(s)..."

# krnxxx.o are from the kernel folder
# drivxxx.o are for drivers.
nasm -f elf32 $KERN_DIR/krnentry.asm -o $BUILD/krnent.o
nasm -f elf32 $DRIV_DIR/interrupt.asm -o $BUILD/drivinterrupt.o
gcc -m32 -c $KERN_DIR/kernel.c -o $BUILD/krnc.o -m32 -fno-stack-protector
gcc -m32 -c $DRIV_DIR/ports.c -o $BUILD/drivport.o
gcc -m32 -c $DRIV_DIR/gdt.c -o $BUILD/drivgdt.o
gcc -m32 -c $DRIV_DIR/idt.c -o $BUILD/drividt.o
gcc -m32 -c $DRIV_DIR/pic.c -o $BUILD/drivpic.o
gcc -m32 -c $DRIV_DIR/utils.c -o $BUILD/driv_utils.o
gcc -m32 -c $DRIV_DIR/vga.c -o $BUILD/driv_vga.o
ld -m elf_i386 -T src/linker.ld --allow-multiple-definition $BUILD/krnent.o $BUILD/drivinterrupt.o $BUILD/krnc.o $BUILD/drivport.o $BUILD/drivgdt.o $BUILD/drividt.o $BUILD/drivpic.o $BUILD/driv_utils.o $BUILD/driv_vga.o -o $BUILD/ChoacuryOS.bin -nostdlib

# Check the binary file is x86 multiboot file or not
echo "Checking if BINARY is GRUB-Ready..."
grub-file --is-x86-multiboot $BUILD/ChoacuryOS.bin

# Building the ISO file
echo "Making ISO file"
mkdir -p isodir/boot/grub
cp $BUILD/ChoacuryOS.bin isodir/boot/ChoacuryOS.bin
cp src/boot/grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o $BUILD/ChoacuryOS.iso isodir

# Add some code for when the rest of the code is ready ;-)
echo "Booting into QEMU"
qemu-system-x86_64 -cdrom $BUILD/ChoacuryOS.iso
