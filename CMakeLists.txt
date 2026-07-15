cmake_minimum_required(VERSION 3.20)

project(UEFI C)

set(CMAKE_C_COMPILER clang-18)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(main.efi
    src/main.c
    src/Globals.c
    src/Console.c
    src/Serial.c
    src/Formatter.c
    src/Logger.c
    src/Screen.c
    src/Physics.c
    src/Render.c
    src/Game.c
    src/Tsc.c
    src/Keyboard/Keyboard.c
    src/Keyboard/Usb.c
    src/Keyboard/Ps2.c
)

target_include_directories(main.efi PRIVATE
    include
    include/Edk2
    include/Edk2/X64
)

target_compile_options(main.efi PRIVATE
    -target x86_64-unknown-windows
    -O2
    -ffast-math
    -ffreestanding
    -fshort-wchar
    -mno-red-zone
)

target_link_options(main.efi PRIVATE
    -target x86_64-unknown-windows
    -nostdlib
    -fuse-ld=lld-link
    -Wl,-entry:efi_main
    -Wl,-subsystem:efi_application
)
