clear
# Start making stuff
echo "Compiling file(s)..."
nasm -f bin src/boot/boot.asm -o boot.bin
nasm -f bin src/kernel/kernel.asm -o kernasm.bin
cat "boot.bin" "kernasm.bin" > Choacury.flp
# Add some code for when the rest of the code is ready ;-)
echo "Booting into QEMU"
qemu-system-x86_64 -fda Choacury.flp
