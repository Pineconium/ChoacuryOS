#!/bin/bash

make && ./create-disk.sh && qemu-system-x86_64 -hda build/ChoacuryOS.img -serial stdio