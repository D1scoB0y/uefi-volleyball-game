#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <Uefi.h>
#include <Protocol/UsbIo.h>

enum {
    KEY_ESC,
    KEY_A,
    KEY_W,
    KEY_D,
    KEY_LEFT,
    KEY_UP,
    KEY_RIGHT,
    KEY_COUNT
};

typedef enum {
    KEYBOARD_TYPE_UNDEFINED,
    KEYBOARD_TYPE_USB,
    KEYBOARD_TYPE_PS2
} KEYBOARD_TYPE;

typedef struct {
    KEYBOARD_TYPE Type;
    BOOLEAN Keys[KEY_COUNT];
} KEYBOARD;

VOID
KeyboardFree(KEYBOARD *Kb);

EFI_STATUS
KeyboardInit(KEYBOARD *Kb);

#endif
