#!/bin/bash

MOUNT_DIR=build/mount
DISK_PATH=build/ChoacuryOS.img
DISK_SIZE=$((50 * 1024 * 1024))

test -f $DISK_PATH
SHOULD_BUILD=$?

if (($SHOULD_BUILD)); then
	# create empty disk image
	truncate -s 0 $DISK_PATH
	dd if=/dev/zero of=$DISK_PATH bs=512 count=$(($DISK_SIZE / 512))

	# create partition table
	parted --script $DISK_PATH	 \
		mklabel gpt              \
		mkpart boot 1M 2M        \
		set 1 bios_grub on       \
		mkpart root ext2 2M 100%
fi

# create loop device
LOOP_DEV=$(sudo losetup --show  -fP $DISK_PATH || exit 1)
PARTITION1=${LOOP_DEV}p1
PARTITION2=${LOOP_DEV}p2
if [ ! -b $PARTITION1 ] || [ ! -b $PARTITION2 ]; then
	echo "Failed to probe partitions for disk image." >&2
	sudo losetup -d $LOOP_DEV
	exit 1
fi

if (($SHOULD_BUILD)); then
	# create root filesystem
	sudo mkfs.fat $PARTITION2
fi

# mount root filesystem
mkdir -p $MOUNT_DIR
sudo mount $PARTITION2 "$MOUNT_DIR"

if (($SHOULD_BUILD)); then
	# install grub
	sudo grub-install						\
		--no-floppy							\
		--target=i386-pc					\
		--modules="normal ext2 multiboot"	\
		--boot-directory="$MOUNT_DIR/boot"	\
		$LOOP_DEV

	sudo mkdir -p "$MOUNT_DIR/boot/grub"
	sudo cp src/boot/grub.cfg $MOUNT_DIR/boot/grub/
fi

# copy kernel to boot
sudo cp build/ChoacuryOS.bin $MOUNT_DIR/boot/
# copy fonts to /
sudo cp Unifont.psf $MOUNT_DIR/
sudo cp test.bmp $MOUNT_DIR/

# cleanup
sudo umount "$MOUNT_DIR"
sudo losetup -d $LOOP_DEV
