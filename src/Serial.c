#include <Globals.h>
#include <Serial.h>
#include <Protocol/SerialIo.h>

STATIC EFI_SERIAL_IO_PROTOCOL *Serial = NULL;

EFI_STATUS
SerialInit()
{
    EFI_GUID SerialGuid = EFI_SERIAL_IO_PROTOCOL_GUID;
    return ST->BootServices->LocateProtocol(
        &SerialGuid,
        NULL,
        (VOID**) &Serial
    );
}

VOID
SerialWrite(CONST CHAR8 *Msg, UINTN Len)
{
    if (!Serial) return;

    UINTN Len_ = Len - 1;
    Serial->Write(Serial, &Len_, (VOID*) Msg);
}
