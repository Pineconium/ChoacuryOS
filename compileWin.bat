@echo off
title Choacury Compiler WINDOWS version
rem Start making stuff
echo Compiling file(s)...
wsl nasm -f elf32 src/kernel/krnentry.asm -o krnent.o
wsl gcc -m32 -c src/kernel/kernel.c -o krnc.o
wsl gcc -m32 -c src/drivers/ports.c -o drivport.o
wsl gcc -m32 -c src/drivers/idt.c -o drividt.o
wsl gcc -m32 -c src/drivers/utils.c -o driv_utils.o
wsl gcc -m32 -c src/drivers/vga.c -o driv_vga.o
wsl ld -m elf_i386 -T src/linker.ld --allow-multiple-definition -o kernel krnent.o krnc.o drivport.o drividt.o driv_utils.o driv_vga.o
echo Booting into QEMU
qemu-system-x86_64 -kernel kernel
