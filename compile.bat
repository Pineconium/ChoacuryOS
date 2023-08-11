@echo off
title Choacury Compiler WINDOWS version
clear
rem Start making stuff
echo Compiling file(s)...
rem krnxxx.o are from the kernel folder
rem drivxxx.o are for drivers.
wsl nasm -f elf32 src/kernel/krnentry.asm -o krnent.o
wsl gcc -m32 -c src/kernel/kernel.c -o krnc.o
wsl gcc -m32 -c src/drivers/ports.c -o drivport.o
wsl gcc -m32 -c src/drivers/idt.c -o drividt.o
wsl gcc -m32 -c src/drivers/utils.c -o driv_utils.o
wsl gcc -m32 -c src/drivers/vga.c -o driv_vga.o
wsl ld -m elf_i386 -T src/linker.ld --allow-multiple-definition krnent.o krnc.o drivport.o drividt.o driv_utils.o driv_vga.o -o ChoacuryOS.bin -nostdlib
rem Check the binary file is x86 multiboot file or not
echo Checking if BINARY is GRUB-Ready...
wsl grub-file --is-x86-multiboot ChoacuryOS.bin
rem Building the ISO file
echo Making ISO file
wsl mkdir -p isodir/boot/grub
wsl cp ChoacuryOS.bin isodir/boot/ChoacuryOS.bin
wsl cp src/boot/grub.cfg isodir/boot/grub/grub.cfg
wsl grub-mkrescue -o ChoacuryOS.iso isodir
rem Add some code for when the rest of the code is ready ;-)
echo Booting into QEMU...
qemu-system-x86_64 -cdrom ChoacuryOS.iso
