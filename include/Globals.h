#ifndef _RUNTIME_H
#define _RUNTIME_H

#include <Uefi.h>

extern EFI_HANDLE IH;
extern EFI_SYSTEM_TABLE *ST;

VOID
GlobalsSet(
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
);

#endif
