#!/bin/bash

# Set the source files
SOURCE_DIR="kernel"
SOURCE_FILES=("boot/32bswitch.asm" "boot/bootloader.asm" "kernel/start.asm" "kernel/tty.c" "kernel/gdt.c" "kernel/idt.c" "kernel/paging.c" "kernel/timer.c" "kernel/interrupt.s")

# Set the compiler and linker flags
CFLAGS="-m32 -ffreestanding -Wall -Wextra -fno-pie -nostdlib -nostdinc -nostartfiles -nodefaultlibs -I include"
LDFLAGS="-melf_i386 -T linker.ld"

# Compile each source file
for file in "${SOURCE_FILES[@]}"
do
    if [[ "$file" == *.c ]]
    then
        gcc $CFLAGS -c $file -o bin/$(basename "$file").o
    elif [[ "$file" == *.asm ]]
    then
        nasm -f elf32 $file -o bin/$(basename "$file").o
    elif [[ "$file" == *.s ]]
    then
        gcc $CFLAGS -c $file -o bin/$(basename "$file").o
    fi
done

# Link the object files
ld $LDFLAGS bin/*.o -o bin/kernel.bin

echo "Compilation and linking completed!"
