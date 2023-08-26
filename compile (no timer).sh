#!/bin/bash

# List of required object files
required_objs=(
    "krnent.o"      
    "driv_interpasm.o"
    "krnc.o"
    "driv_port.o"
    "driv_idt.o"
    "driv_utils.o"
    "driv_vga.o"
    "driv_isrc.o"
)

# Compiles the files from /src/ as .o files
echo "Compiling file(s)..."
nasm -f elf32 src/kernel/krnentry.asm -o krnent.o
nasm -f elf32 src/drivers/isr.asm -o driv_interpasm.o
gcc -m32 -c src/kernel/kernel.c -o krnc.o
gcc -m32 -c src/drivers/ports.c -o driv_port.o
gcc -m32 -c src/drivers/idt.c -o driv_idt.o
gcc -m32 -c src/drivers/utils.c -o driv_utils.o
gcc -m32 -c src/drivers/vga.c -o driv_vga.o
gcc -m32 -c src/drivers/isr.c -o driv_isrc.o
ld -m elf_i386 -T src/linker.ld --allow-multiple-definition krnent.o krnc.o driv_port.o driv_idt.o driv_utils.o driv_vga.o driv_isrc.o driv_interpasm.o -o ChoacuryOS.bin -nostdlib

# Checks if a .o file is missing..
check_file_exists() {
    if [ ! -f "$1" ]; then
        # Fancy error string formatting LOL.
        printf "\033[91m*FATAL*\033[0m $1 is missing. Check terminal output and fix the source file for $1 for any bugs! "
        echo "  "
        exit 1
    fi
}

# Check if any of the required object files are missing
for obj_file in "${required_objs[@]}"; do
    check_file_exists "$obj_file"
done

# Check the binary file is x86 multiboot file or not...
echo "Checking if BINARY is GRUB-Ready..."
grub-file --is-x86-multiboot ChoacuryOS.bin

# Building the ISO file...
echo "Making ISO file"
mkdir -p isodir/boot/grub
cp ChoacuryOS.bin isodir/boot/ChoacuryOS.bin
cp src/boot/grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o ChoacuryOS.iso isodir

# Add some code for when the rest of the code is ready ;-)
echo "Booting into QEMU"
qemu-system-x86_64 -cdrom ChoacuryOS.iso
