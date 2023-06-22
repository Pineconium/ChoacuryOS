#!/bin/bash

set -xe

mkdir -p build

# bootloader
nasm -f bin src/bootloader/boot.asm -o build/boot.o
nasm -f elf64 src/bootloader/loader.asm -o build/loader.o

# kernel
gcc -Wall -Wextra -pedantic -ggdb -std=c99 -m64 -ffreestanding -c src/kernel/main.c -o build/kernel.o

# link loader to kernel
ld -Ttext THE_KERNEL_ADRESS -o build/kernel.elf build/loader.o build/kernel.o
objcopy -R .note -R .comment -S -O binary build/kernel.elf build/kernel.bin

# create the floppy img
dd if=/dev/zero of=build/os.img bs=512 count=2880
dd if=build/boot.bin of=build/os.img conv=notrunc
dd if=build/kernel.bin of=build/os.img conv=notrunc bs=512 seek=1
