#ifndef _SERIAL_H
#define _SERIAL_H

#include <Uefi.h>

EFI_STATUS
SerialInit();

VOID
SerialWrite(CONST CHAR8 *Msg, UINTN Len);

#endif
