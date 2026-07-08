#include <Globals.h>

VOID
ConsoleWrite(CHAR16 *Msg)
{
    ST->ConOut->OutputString(ST->ConOut, Msg);
}
