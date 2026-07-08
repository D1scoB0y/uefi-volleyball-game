#ifndef _USB_H
#define _USB_H

#include <Keyboard.h>

VOID
UsbKeyboardInfoFree(VOID);

EFI_STATUS
UsbKeyboardInit(KEYBOARD *Kb);

#endif
