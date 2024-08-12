@echo off
title Choacury Compiler
wsl ./compile.sh
pause
qemu -hda build/ChoacuryOS.img -display sdl
