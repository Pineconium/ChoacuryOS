@echo off
title Choacury compiler for WINDOWS
rem Start making stuff
echo Compiling file(s)...
nasm -f bin src/boot/boot.asm -o boot.bin
nasm -f bin src/kernel/kernel.asm -o kernasm.bin
copy /b boot.bin + kernasm.bin Choacury.flp
rem Add some code for when the rest of the code is ready ;-)
echo Booting into QEMU
qemu-system-x86_64 -fda Choacury.flp
