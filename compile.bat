@echo off
title Choacury Compiler
wsl make iso
echo Booting into QEMU...
qemu-system-x86_64 -cdrom .\build\ChoacuryOS.iso
