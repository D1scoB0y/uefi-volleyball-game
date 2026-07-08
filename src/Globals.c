#include <Uefi.h>

EFI_HANDLE IH = NULL;
EFI_SYSTEM_TABLE *ST = NULL;

VOID
GlobalsSet(
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
)
{
    IH = ImageHandle;
    ST = SystemTable;
}
