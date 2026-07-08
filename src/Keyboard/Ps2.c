#include <Globals.h>
#include <Keyboard.h>
#include <Logger.h>

#define PS2_STATUS_PORT     0x64
#define PS2_DATA_PORT       0x60

STATIC BOOLEAN E0Prefix = FALSE;

STATIC
inline
UINT8
IoRead8(UINT16 port)
{
    UINT8 value;

    __asm__ volatile (
        "inb %w1, %0"
        : "=a"(value)
        : "Nd"(port)
        : "memory"
    );

    return value;
}

STATIC
inline
VOID
IoWrite8(UINT16 port, UINT8 value)
{
    __asm__ volatile (
        "outb %0, %w1"
        :
        : "a"(value), "Nd"(port)
        : "memory"
    );
}

STATIC
VOID
HandleScanCode(BOOLEAN *Keys, UINT8 Code)
{
    BOOLEAN Pressed = !(Code & 0x80);
    UINT8 Scan = Code & 0x7F;

    if (Code == 0xE0) {
        E0Prefix = TRUE;
        return;
    }

    if (!E0Prefix) {
        switch (Scan) {
        case 0x1E: // A
            Keys[KEY_A] = Pressed;
            break;

        case 0x11: // W
            Keys[KEY_W] = Pressed;
            break;

        case 0x20: // D
            Keys[KEY_D] = Pressed;
            break;
        }
    } else {
        switch (Scan) {
        case 0x4B: // Left
            Keys[KEY_LEFT] = Pressed;
            break;

        case 0x48: // Up
            Keys[KEY_UP] = Pressed;
            break;

        case 0x4D: // Right
            Keys[KEY_RIGHT] = Pressed;
            break;
        }

        E0Prefix = FALSE;
    }
}

STATIC
VOID
EFIAPI
Callback(
    EFI_EVENT _,
    VOID *Context
)
{
    BOOLEAN *Keys = Context;
    while (IoRead8(PS2_STATUS_PORT) & 1) {
        UINT8 Code = IoRead8(PS2_DATA_PORT);
        HandleScanCode(Keys, Code);
    }
}

EFI_STATUS
Ps2KeyboardInit(KEYBOARD *Kb)
{
    IoWrite8(0x64, 0xAA);

    while(!(IoRead8(0x64) & 1));

    UINT8 Result = IoRead8(0x60);

    if (Result != 0x55) return EFI_NOT_FOUND;

    EFI_STATUS Status;
    EFI_HANDLE *Handles;
    UINTN HandleCount;

    EFI_GUID SimpleTextInputGuid = EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID;

    Status = ST->BootServices->LocateHandleBuffer(
        ByProtocol,
        &SimpleTextInputGuid,
        NULL,
        &HandleCount,
        &Handles
    );
    if (EFI_ERROR(Status)) {
        Printf(L"Error locating handle buffer: %r\r\n", Status);
        return Status;
    }

    for (UINTN i = 0; i < HandleCount; i++) {
        ST->BootServices->DisconnectController(
            Handles[i],
            NULL,
            NULL
        );
    }

    ST->BootServices->FreePool(Handles);

    EFI_EVENT Ps2KeyboardTimer;
    Status = ST->BootServices->CreateEvent(
        EVT_TIMER | EVT_NOTIFY_SIGNAL,
        TPL_CALLBACK,
        Callback,
        Kb->Keys,
        &Ps2KeyboardTimer
    );
    if (EFI_ERROR(Status)) {
        Printf(L"Error creating event: %r\r\n", Status);
        return Status;
    };

    Status = ST->BootServices->SetTimer(
        Ps2KeyboardTimer,
        TimerPeriodic,
        80000 // 8 ms
    );
    if (EFI_ERROR(Status)) {
        Printf(L"Error setting timer: %r\r\n", Status);
        return Status;
    };

    Kb->Type = Result == KEYBOARD_TYPE_PS2;

    return EFI_SUCCESS;
}
