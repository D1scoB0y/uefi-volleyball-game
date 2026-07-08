#!/bin/bash

./build.sh

sudo umount /dev/sda1

sudo parted /dev/sda --script \
    mklabel gpt \
    mkpart ESP fat32 1MiB 100% \
    set 1 esp on

sudo mkfs.fat -F32 /dev/sda1

sudo mkdir -p /mnt/efiusb
sudo mount /dev/sda1 /mnt/efiusb

sudo mkdir -p /mnt/efiusb/EFI/BOOT

sudo cp build/main.efi /mnt/efiusb/EFI/BOOT/BOOTX64.EFI

sync
sudo umount /mnt/efiusb
