#include <Keyboard.h>
#include <Logger.h>

#include "Uefi/UefiBaseType.h"
#include "Usb.h"
#include "Ps2.h"

VOID
KeyboardFree(KEYBOARD *Kb)
{
    if (Kb->Type == KEYBOARD_TYPE_USB)
        UsbKeyboardInfoFree();

    Kb->Type = KEYBOARD_TYPE_UNDEFINED;
}

EFI_STATUS
KeyboardInit(KEYBOARD *Kb)
{
    Printf(L"Trying to find USB keyboard...\r\n");

    EFI_STATUS Status;

    Status = UsbKeyboardInit(Kb);
    if (!EFI_ERROR(Status)) return EFI_SUCCESS;

    Printf(L"Error initializing usb keyboard: %r\r\n", Status);

    Printf(L"Trying to find PS/2 keyboard...\r\n");

    Status = Ps2KeyboardInit(Kb);
    if (EFI_ERROR(Status)) {
        Printf(L"Error initializing PS/2 keyboard: %r\r\n", Status);
        return Status;
    }

    Printf(L"PS/2 keyboard was found!\r\n");

    return EFI_SUCCESS;
}
