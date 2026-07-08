#!/bin/bash

BUILD_DIR="build"
SRC_DIR="src"

CFLAGS="\
	-target x86_64-unknown-windows \
	-Iinclude \
	-Iinclude/Edk2 \
    -Iinclude/Edk2/X64 \
    -O2 \
    -ffast-math \
	-ffreestanding \
	-fshort-wchar \
	-mno-red-zone"

LDFLAGS="\
	-target x86_64-unknown-windows \
	-nostdlib \
	-Wl,-entry:efi_main \
	-Wl,-subsystem:efi_application \
	-fuse-ld=lld-link"

mkdir -p $BUILD_DIR

clang-18 $CFLAGS -o $BUILD_DIR/main.o -c $SRC_DIR/main.c
clang-18 $CFLAGS -o $BUILD_DIR/Globals.o -c $SRC_DIR/Globals.c
clang-18 $CFLAGS -o $BUILD_DIR/Console.o -c $SRC_DIR/Console.c
clang-18 $CFLAGS -o $BUILD_DIR/Serial.o -c $SRC_DIR/Serial.c
clang-18 $CFLAGS -o $BUILD_DIR/Formatter.o -c $SRC_DIR/Formatter.c
clang-18 $CFLAGS -o $BUILD_DIR/Logger.o -c $SRC_DIR/Logger.c
clang-18 $CFLAGS -o $BUILD_DIR/Screen.o -c $SRC_DIR/Screen.c
clang-18 $CFLAGS -o $BUILD_DIR/Physics.o -c $SRC_DIR/Physics.c
clang-18 $CFLAGS -o $BUILD_DIR/Render.o -c $SRC_DIR/Render.c
clang-18 $CFLAGS -o $BUILD_DIR/Game.o -c $SRC_DIR/Game.c
clang-18 $CFLAGS -o $BUILD_DIR/Tsc.o -c $SRC_DIR/Tsc.c
clang-18 $CFLAGS -o $BUILD_DIR/Keyboard.o -c $SRC_DIR/Keyboard/Keyboard.c
clang-18 $CFLAGS -o $BUILD_DIR/Usb.o -c $SRC_DIR/Keyboard/Usb.c
clang-18 $CFLAGS -o $BUILD_DIR/Ps2.o -c $SRC_DIR/Keyboard/Ps2.c

clang-18 $LDFLAGS -o $BUILD_DIR/main.efi \
    $BUILD_DIR/main.o \
    $BUILD_DIR/Globals.o \
    $BUILD_DIR/Console.o \
    $BUILD_DIR/Serial.o \
    $BUILD_DIR/Formatter.o \
    $BUILD_DIR/Logger.o \
    $BUILD_DIR/Screen.o \
    $BUILD_DIR/Keyboard.o \
    $BUILD_DIR/Usb.o \
    $BUILD_DIR/Ps2.o \
    $BUILD_DIR/Tsc.o \
    $BUILD_DIR/Game.o \
    $BUILD_DIR/Physics.o \
    $BUILD_DIR/Render.o
