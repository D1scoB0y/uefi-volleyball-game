#!/bin/bash

./build.sh

mkdir -p image/efi/boot
cp -f build/main.efi image/efi/boot/bootx64.efi

cp /usr/share/OVMF/OVMF_CODE_4M.fd ./OVMF_CODE_4M.fd
cp /usr/share/OVMF/OVMF_VARS_4M.fd ./OVMF_VARS_4M.fd

qemu-system-x86_64 \
    -machine q35 \
    -enable-kvm \
    -cpu host \
    -net none \
    -drive if=pflash,format=raw,readonly=on,file=OVMF_CODE_4M.fd \
    -drive if=pflash,format=raw,file=OVMF_VARS_4M.fd \
    -drive file=fat:rw:image,format=raw \
    -device qemu-xhci \
    -device usb-kbd \
    -serial stdio
