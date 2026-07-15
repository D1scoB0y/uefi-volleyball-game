# UEFI Volleyball

A simple 2D volleyball game that runs directly as a native UEFI application without an operating system.

<p align="center">
  <img src="docs/images/qemu.png" alt="Gameplay" width="800">
</p>

## Building

The project uses CMake and Clang.

```bash
cmake -B build
cmake --build build
```

After a successful build, the generated `.efi` executable can be found in the build directory.

## Running in QEMU

The project includes a script for launching the application in QEMU with OVMF firmware.

```bash
./qemu.sh
```

The script creates the required disk image, copies the EFI application, and starts QEMU with UEFI firmware.

## Running on Real Hardware

A helper script is provided to prepare a bootable USB flash drive. Fill with yours USB device!!!

```bash
./usb.sh
```

After the script finishes, reboot the computer, open the UEFI boot menu, select the USB drive, and start the application.

The game has been tested on real hardware and supports both USB and PS/2 (set 1) keyboards.

<p align="center">
  <img src="docs/images/real.png" alt="Running on real hardware" width="800">
</p>
